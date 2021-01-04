#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"

#include "Scene/JigsawScenesObserver.h"
#include "Entities/JigsawEntityClusterService.h"
#include "Assets/DataAssets.h"
#include "Physics/TransformMachinePiece.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Scene;
using namespace Jigsaw::Entities;
using namespace Jigsaw;

#define FAB_COUNT 300

namespace ProjectTests {
	class MockJsonAsset : public Jigsaw::Assets::JsonAsset {
	public:
		MockJsonAsset(Jigsaw::Assets::AssetDescriptor& desc, void* object) : Jigsaw::Assets::JsonAsset(desc) {
			this->object = object;
		}
	};

	TEST_CLASS(JigsawScenesObserverTest)
	{

		static Ref<JigsawScenesObserver> observer;
		static Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine> machine_ref;
		static Ref<JigsawEntityClusterService> cluster_service;
		static Ref<JigsawScene> scene;

		static Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine> GetMachine() {
			Jigsaw::Assets::AssetDescriptor desc;
			desc.id = Jigsaw::System::UID::Create();
			desc.fully_qualified_type_name = Jigsaw::Util::etype_info::Id<Jigsaw::Machines::JigsawMachine>().GetQualifiedName();

			Jigsaw::Machines::JigsawMachine* machine = new Jigsaw::Machines::JigsawMachine;
			machine->machine_id = Jigsaw::System::UID::Create();
			machine->name = "test_machine";
			machine->pieces.push_back(new Jigsaw::Physics::TransformMachinePiece);

			// initializing 'SerializedRef' for JigsawMachine
			Jigsaw::Assets::JsonAsset* asset = new MockJsonAsset(desc, machine);
			Ref<Jigsaw::Assets::JsonAsset> machine_asset = Ref<Jigsaw::Assets::JsonAsset>(asset);
			return Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>(machine_asset);
		}

		TEST_CLASS_INITIALIZE(Initialize) {
			// initializing services and the descriptors/assets
			cluster_service = MakeRef<JigsawEntityClusterService>();
			observer = MakeRef<JigsawScenesObserver>(cluster_service);
			Jigsaw::System::UID scene_id = Jigsaw::System::UID::Create();

			machine_ref = GetMachine();
			std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>> vector;
			vector.push_back(machine_ref);

			// intializing fabrication arguments
			Jigsaw::Machines::MULTI_FABRICATION_ARGS args;
			Jigsaw::Machines::FABRICATION_ARGS fab_args_a, fab_args_b;
			Transform t_a, t_b;

			t_a.position = Vector3(1.5, 2.2, 3.7);
			t_a.scale = Vector3(1, -1, 3);
			t_b.scale = Vector3(1.5, 2.2, 3.7);

			fab_args_a.WithValue(std::move(t_a));
			fab_args_b.WithValue(std::move(t_b));

			args.entity_fabrication_args.push_back(std::move(fab_args_a));
			args.entity_fabrication_args.push_back(std::move(fab_args_b));

			std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS> scene_args;
			scene_args.insert(std::make_pair(machine_ref->machine_id, args));

			// building the scene with the fabrication arguments pre-loaded
			scene = MakeRef<JigsawScene>(scene_id, std::move(scene_args), std::move(vector));
		}

		TEST_CLASS_CLEANUP(Cleanup) {
			machine_ref.Reset();
			scene.reset();
		}

		TEST_METHOD(TestActivatedSceneGeneratesMachineEntities) {
			observer->ActivateScene(scene);

			const Jigsaw::System::Signature signature = *machine_ref->GetSignature().get();
			Jigsaw::Entities::JigsawEntityCluster& cluster = (*cluster_service)[signature];
			Assert::AreEqual(2, (int)cluster.GetTotalCount());
			Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver> machine_observer = observer->GetMachineObserver(machine_ref->machine_id);
			const std::vector<Jigsaw::System::UID>& entity_ids = machine_observer->GetEntityIds();

			JigsawEntity entity_a = cluster.FetchEntity(entity_ids.at(0));
			JigsawEntity entity_b = cluster.FetchEntity(entity_ids.at(1));

			Transform transform_a = entity_a.GetMemberData<Transform>();
			Transform transform_b = entity_b.GetMemberData<Transform>();

			Assert::IsTrue(Vector3(1.5, 2.2, 3.7) == transform_a.position);
			Assert::IsTrue(Vector3(1, -1, 3) == transform_a.scale);
			Assert::IsTrue(Vector3(1.5, 2.2, 3.7) == transform_b.scale);
			Assert::IsTrue(Vector3(0, 0, 0) == transform_b.position);

			observer->DeactivateScene(scene->GetUID());
		}

		TEST_METHOD(TestDeactivatedSceneDeletesMachineEntities) {
			observer->ActivateScene(scene);
			observer->DeactivateScene(scene->GetUID());

			const Jigsaw::System::Signature signature = *machine_ref->GetSignature().get();
			Jigsaw::Entities::JigsawEntityCluster& cluster = (*cluster_service)[signature];
			Assert::AreEqual(0, (int)cluster.GetTotalCount());
		}

		TEST_METHOD(TestFetchMachineObserverAndFabricateEntity) {
			observer->ActivateScene(scene);

			Jigsaw::Ref<Jigsaw::Machines::JigsawMachineObserver> machine_observer = observer->GetMachineObserver(machine_ref->machine_id);
			Jigsaw::Machines::FABRICATION_ARGS args;
			Transform t;
			t.position = Vector3(11, -222, 77);
			t.scale = Vector3(1.5, 1.5, 2.2);
			t.rotation = Quaternion(1, 1, 1, 1).Conjugate();
			args.WithValue(std::move(t));
			Jigsaw::Entities::JigsawEntity e = machine_observer->FabricateEntity(args);

			Jigsaw::Entities::JigsawEntityCluster& cluster = (*cluster_service)[machine_observer->GetMachineSignature()];

			// ensure that not only the original Entity data are loaded, but the fabricated entity as well
			Assert::AreEqual(3, (int)cluster.GetTotalCount());
			Assert::IsTrue(e.GetMemberData<Transform>() == t);

			for (int i = 0; i < FAB_COUNT; i++) {
				Jigsaw::Entities::JigsawEntity e_a = machine_observer->FabricateEntity(args);
			}

			Assert::AreEqual(3 + FAB_COUNT, (int)cluster.GetTotalCount());
			observer->DeactivateScene(scene->GetUID());
			Assert::AreEqual(0, (int)cluster.GetTotalCount());
		}

		TEST_METHOD(TestOnlyOneMachineEntitiesRemovedOnSceneDeactivation) {
			auto m_ref = GetMachine();
			// activating the original scene
			observer->ActivateScene(scene);
			std::vector<Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine>> vector;
			vector.push_back(m_ref);
			Jigsaw::System::UID s_id = Jigsaw::System::UID::Create();
			Jigsaw::Ref<Jigsaw::Scene::JigsawScene> other_scene = MakeRef<JigsawScene>(s_id, std::map<Jigsaw::System::UID, Jigsaw::Machines::MULTI_FABRICATION_ARGS>(), std::move(vector));

			observer->ActivateScene(other_scene);
			const Ref<Jigsaw::Machines::JigsawMachineObserver>& m_observer = observer->GetMachineObserver(m_ref->machine_id);

			Jigsaw::Entities::JigsawEntity e = m_observer->FabricateEntity();
			JigsawEntityCluster& cluster = (*cluster_service)[m_observer->GetMachineSignature()];
			Assert::AreEqual(3, (int)cluster.GetTotalCount());

			observer->DeactivateScene(scene->GetUID());
			Assert::AreEqual(1, (int)cluster.GetTotalCount());

			Jigsaw::Entities::JigsawEntity e_ = cluster.FetchEntity(e.GetUID());
			Transform t = e_.GetMemberData<Transform>();
			Assert::IsTrue(Transform() == t);
			observer->DeactivateScene(other_scene->GetUID());
		}
	};

	Ref<JigsawScenesObserver> JigsawScenesObserverTest::observer;
	Ref<JigsawEntityClusterService> JigsawScenesObserverTest::cluster_service;
	Ref<JigsawScene> JigsawScenesObserverTest::scene;
	Jigsaw::Assets::SerializedRef<Jigsaw::Machines::JigsawMachine> JigsawScenesObserverTest::machine_ref;
}
#endif
