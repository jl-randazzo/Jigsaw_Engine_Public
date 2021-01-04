#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Scene/JigsawSceneLoader.h"
#include "Assets/AssetDatabase.h"
#include "System/ASYNC_JOB.h"
#include "Ref.h"
#include "TestUtils.h"
#include "MockCommandListExecutor.h"
#include "Machines/JigsawMachine.h"
#include "Marshalling/MarshallingUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Assets;
using namespace Jigsaw::System;
using namespace Jigsaw::Machines;
using namespace Jigsaw::Scene;
using namespace Jigsaw;

#define CHILD_MACHINE_COUNT 4
#define RESOURCE_POOL_COUNT 2

namespace ProjectTests {
	TEST_CLASS(TestJigsawSceneLoader)
	{
		static Ref<AssetDatabaseReadWriter> db;
		static UID alt_scene_id;
		static UID scene_id;
		static Unique<AssetDescriptor[]> machine_descs;
		static Ref<Jigsaw::Graphics::CommandListExecutor> executor;
		static THREAD_SAFE_RESOURCE_POOL resource_pool;

		TEST_CLASS_INITIALIZE(Setup) {
			resource_pool = THREAD_SAFE_RESOURCE_POOL();
			db = GetTestDB();
			ProjectTests::WriteTestTableEntries(db);

			scene_id = UID::Create();
			alt_scene_id = UID::Create();
			machine_descs = Unique<AssetDescriptor[]>(new AssetDescriptor[CHILD_MACHINE_COUNT]);
			for (int i = 0; i < CHILD_MACHINE_COUNT; i++) {
				AssetDescriptor& desc = machine_descs.get()[i];
				desc.id = UID::Create();
				desc.file_path = "./Assets/TestJigsawSceneLoader/machine.json";
				desc.fully_qualified_type_name = "Jigsaw::Machines::JigsawMachine";
				desc.type = ASSET_JSON;
				db->WriteDescriptor(desc);
			}
			Unique<UID[]> machine_ids = UniqueMap<AssetDescriptor, UID>(machine_descs, CHILD_MACHINE_COUNT, [](AssetDescriptor& a) { return a.id; });

			db->WriteSceneMachineAssociations(scene_id, machine_ids, CHILD_MACHINE_COUNT);
			executor = Ref<Jigsaw::Graphics::CommandListExecutor>(new MockCommandListExecutor);

			for (int i = 0; i < RESOURCE_POOL_COUNT; i++) {
				THREAD_SAFE_SYSTEM_RESOURCES sys_resources;
				sys_resources.cmd_list = Ref<Jigsaw::Graphics::CommandList>(new MockCommandList);
				sys_resources.db = MakeRef<AssetDatabaseReader>("./unit_test.db");
				resource_pool.Enqueue(std::move(sys_resources));
			}
			resource_pool.cmd_list_exec = executor;
		}

		TEST_CLASS_CLEANUP(Cleanup) {
			DestroyTestDB(db);
		}

		TEST_METHOD(TestLoadSceneAssets) {
			JigsawSceneLoader scene_l({ scene_id, resource_pool.Split(RESOURCE_POOL_COUNT) });
			Ref<ASYNC_JOB<SCENE_LOAD_RESULT>> scene_load_job = scene_l.LoadScene();

			Assert::IsFalse(scene_load_job->Ready());
			while (!scene_load_job->Ready()) {}
			SCENE_LOAD_RESULT load_result = scene_load_job->Get();

			Assert::IsNotNull(load_result.loaded_scene.get());
			Assert::AreEqual(CHILD_MACHINE_COUNT, (int)load_result.loaded_scene->GetMachines().size());
			Assert::AreEqual(RESOURCE_POOL_COUNT, (int)load_result.recycled_resources.PoolSize());
			resource_pool.Merge(std::move(load_result.recycled_resources));

			std::vector<SerializedRef<JigsawMachine>> machines = load_result.loaded_scene->GetMachines();
			for (SerializedRef<JigsawMachine>& machine : machines) {
				Assert::AreEqual("Luke's Jigsaw Machine", machine->name.c_str());
			}
		}

		TEST_METHOD(TestLoadSceneBringsSerializedMachineData) {
			Jigsaw::System::UID t_scene_id = Jigsaw::System::UIDFromString(T_SCENE_ID);
			JigsawSceneLoader scene_l({ t_scene_id, resource_pool.Split(RESOURCE_POOL_COUNT) });
			Jigsaw::Ref<Jigsaw::System::ASYNC_JOB<Jigsaw::Scene::SCENE_LOAD_RESULT>> load_res = scene_l.LoadScene();
			Jigsaw::Scene::SCENE_LOAD_RESULT result = load_res->Await();
			
			auto args = result.loaded_scene->GetMachineFabricationArgs();
			auto iter = args.find(Jigsaw::System::UIDFromString(T_MACHINE_ID));
			Assert::IsFalse(iter == args.end());
		}

		TEST_METHOD(TestLoadGraphicsAssetsExecutesGraphicsCommandList) {
			Unique<UID[]> machine_ids = Unique<UID[]>(new UID(Jigsaw::System::UIDFromString(T_MACHINE_ID)));

			db->WriteSceneMachineAssociations(alt_scene_id, machine_ids, 1);

			JigsawSceneLoader scene_l({ alt_scene_id, resource_pool.Split(1) });
			auto load_job = scene_l.LoadScene();

			SCENE_LOAD_RESULT result = load_job->Await();

			Assert::AreEqual(1, static_cast<MockCommandListExecutor*>(executor.get())->list_invocation_count);
			resource_pool.Merge(std::move(result.recycled_resources));

			auto queue = resource_pool.GetFilledResources();
			while (!queue.empty()) {
				auto front = queue.front();
				queue.pop();
				resource_pool.Enqueue(std::move(front));
			}
		}
	};

	Ref<AssetDatabaseReadWriter> TestJigsawSceneLoader::db;
	UID TestJigsawSceneLoader::scene_id;
	UID TestJigsawSceneLoader::alt_scene_id;
	Unique<AssetDescriptor[]> TestJigsawSceneLoader::machine_descs;
	Ref<Jigsaw::Graphics::CommandListExecutor> TestJigsawSceneLoader::executor;
	THREAD_SAFE_RESOURCE_POOL TestJigsawSceneLoader::resource_pool;
}
#endif
