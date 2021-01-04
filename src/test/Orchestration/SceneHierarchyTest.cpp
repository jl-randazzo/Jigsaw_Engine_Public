#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Ref.h"
#include "Graphics/RenderContext.h"
#include "MockCommandListExecutor.h"
#include "Orchestration/SceneHierarchy.h"
#include "Entities/JigsawEntityClusterService.h"
#include "Application/ApplicationRootProperties.h"
#include "TestUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProjectTests {
	class MockSceneHierarchy : public Jigsaw::Orchestration::SceneHierarchy {
	public:
		static Jigsaw::Ref<Jigsaw::Orchestration::SceneHierarchy> Init(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context,
			const Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService>& cluster_service) {
			return Jigsaw::Orchestration::SceneHierarchy::Init(render_context, cluster_service);
		}

		static void Destroy() {
			orchestrator.reset();
		}
	};

	TEST_CLASS(SceneHierarchyTest)
	{
		static Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context;
		static Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;
		Jigsaw::Ref<Jigsaw::Orchestration::SceneHierarchy> orchestrator;

		TEST_CLASS_INITIALIZE(Setup) {
			auto db = GetTestDB();
			WriteTestTableEntries(db);

			render_context = Jigsaw::Ref<Jigsaw::Graphics::RenderContext>(new MockRenderContext);
			cluster_service = Jigsaw::MakeRef<Jigsaw::Entities::JigsawEntityClusterService>();

			ApplicationRootProperties properties;
			properties.db_connection = "./unit_test.db";
			properties.root_scene_id = Jigsaw::System::UIDFromString("f1207d0a-4e50-4f6d-8ec6-d4dd4abbdddc");
			const_cast<ApplicationRootProperties&>(ApplicationRootProperties::Get()) = properties;
		}

		TEST_METHOD_CLEANUP(MethodCleanup) {
			for (auto cluster_it : *cluster_service) {
				auto cluster = cluster_it.second;
				auto iter = cluster->GetNodeIterator(0);
				while (cluster->GetTotalCount() > 0) {
					auto entity = *iter;
					cluster->RemoveEntity(entity.GetUID());
				}
			}
			MockSceneHierarchy::Destroy();
			orchestrator.reset();
		}

		TEST_CLASS_CLEANUP(Cleanup) {
			cluster_service.reset();
			render_context.reset();
		}

		TEST_METHOD(TestInitializeSceneContext) {
			orchestrator = MockSceneHierarchy::Init(render_context, cluster_service);
			size_t array_size;
			Jigsaw::Unique<Jigsaw::System::UID[]> active_ids = orchestrator->GetActiveSceneIds(array_size);
			Assert::AreEqual(0, (int)array_size);
			orchestrator->LoadAndActivateScene(ApplicationRootProperties::Get().root_scene_id);

			active_ids = orchestrator->GetActiveSceneIds(array_size);
			Assert::AreEqual(1, (int)array_size);
		}

		TEST_METHOD(TestAwaitScene) {
			orchestrator = MockSceneHierarchy::Init(render_context, cluster_service);
			Jigsaw::Ref<const Jigsaw::System::EVENT_LISTENER> listener =
				Jigsaw::Orchestration::SceneHierarchy::LoadScene(ApplicationRootProperties::Get().root_scene_id);

			Assert::IsFalse(listener->Check());

			int i = 0;
			while (!listener->Check()) {
				std::this_thread::sleep_for(std::chrono::microseconds(16667));
				orchestrator->Update();
				i++;
				if (i > 5) {
					Assert::Fail(L"Infinite loop appears in Scene await operation");
				}
			}

			size_t array_size;
			Jigsaw::Unique<Jigsaw::System::UID[]> active_ids = orchestrator->GetActiveSceneIds(array_size);
			Assert::AreEqual(1, (int)array_size);
			Assert::IsTrue(listener->Check());
		}

	};
	Jigsaw::Ref<Jigsaw::Graphics::RenderContext> SceneHierarchyTest::render_context;
	Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> SceneHierarchyTest::cluster_service;
}
#endif
