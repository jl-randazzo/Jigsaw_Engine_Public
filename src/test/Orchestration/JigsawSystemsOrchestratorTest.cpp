#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Orchestration/JigsawSystemsOrchestrator.h"
#include "Systems/TransformSystemMock.h"
#include "Time/TimeStep.h"
#include "Ref.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define T_ENTITY_COUNT 1000

namespace ProjectTests {
	TEST_CLASS(JigsawSystemsOrchestratorTest)
	{
		static Jigsaw::Ref<Jigsaw::Orchestration::JigsawSystemsOrchestrator> orchestrator;
		static Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;
		static Jigsaw::System::UID* id_array_a;
		static Jigsaw::System::UID* id_array_b;
		static Jigsaw::Entities::JigsawEntityCluster* _cluster_a;
		static Jigsaw::Entities::JigsawEntityCluster* _cluster_b;

		static void FabricateMany(Jigsaw::Entities::JigsawEntityCluster& cluster, Jigsaw::System::UID* uids) {
			for (int i = 0; i < T_ENTITY_COUNT; i++) {
				uids[i] = cluster.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL).GetUID();
			}
		};

		TEST_CLASS_INITIALIZE(Setup) {
			cluster_service = Jigsaw::MakeRef<Jigsaw::Entities::JigsawEntityClusterService>();
			orchestrator = Jigsaw::MakeRef<Jigsaw::Orchestration::JigsawSystemsOrchestrator>(cluster_service);

			Jigsaw::System::SignatureBuilder builder_a, builder_b;
			builder_a.AddType(Jigsaw::Util::etype_info::Id<Transform>()).AddType(Jigsaw::Util::etype_info::Id<Jigsaw::System::UID>());
			builder_b.AddType(Jigsaw::Util::etype_info::Id<Transform>()).AddType(Jigsaw::Util::etype_info::Id<Jigsaw::Time::TimeStep>());
			Jigsaw::Unique<Jigsaw::System::Signature> sig_a = builder_a.Build();
			Jigsaw::Unique<Jigsaw::System::Signature> sig_b = builder_b.Build();

			Jigsaw::Entities::JigsawEntityCluster& cluster_a = (*cluster_service)[*sig_a.get()];
			Jigsaw::Entities::JigsawEntityCluster& cluster_b = (*cluster_service)[*sig_b.get()];

			id_array_a = new Jigsaw::System::UID[T_ENTITY_COUNT];
			id_array_b = new Jigsaw::System::UID[T_ENTITY_COUNT];

			FabricateMany(cluster_a, id_array_a);
			FabricateMany(cluster_b, id_array_b);

			_cluster_a = &cluster_a;
			_cluster_b = &cluster_b;
		}

		TEST_METHOD(TestEntitiesAlteredOnSystemsUpdate) {
			Transform a_init_transforms[T_ENTITY_COUNT];
			Transform b_init_transforms[T_ENTITY_COUNT];

			for (int i = 0; i < T_ENTITY_COUNT; i++) {
				a_init_transforms[i] = (*_cluster_a).FetchEntity(id_array_a[i]).GetMemberData<Transform>();
				b_init_transforms[i] = (*_cluster_b).FetchEntity(id_array_b[i]).GetMemberData<Transform>();
			}

			orchestrator->Update();

			for (int i = 0; i < T_ENTITY_COUNT; i++) {
				Assert::AreEqual(a_init_transforms[i].position.x + 5, (*_cluster_a).FetchEntity(id_array_a[i]).GetMemberData<Transform>().position.x);
				Assert::AreEqual(b_init_transforms[i].position.x + 5, (*_cluster_b).FetchEntity(id_array_b[i]).GetMemberData<Transform>().position.x);
			}
		}
	};

	Jigsaw::Ref<Jigsaw::Orchestration::JigsawSystemsOrchestrator> JigsawSystemsOrchestratorTest::orchestrator;
	Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> JigsawSystemsOrchestratorTest::cluster_service;
	Jigsaw::System::UID* JigsawSystemsOrchestratorTest::id_array_a;
	Jigsaw::System::UID* JigsawSystemsOrchestratorTest::id_array_b;
	Jigsaw::Entities::JigsawEntityCluster* JigsawSystemsOrchestratorTest::_cluster_a;
	Jigsaw::Entities::JigsawEntityCluster* JigsawSystemsOrchestratorTest::_cluster_b;
}
#endif
