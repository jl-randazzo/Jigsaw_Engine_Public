#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Systems/RootSystem.h"
#include "System/Signature.h"
#include "Physics/Transform.h"
#include "Time/TimeStep.h"
#include "Entities/JigsawEntityClusterService.h"
#include "TransformSystemMock.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProjectTests {

	TEST_CLASS(RootSystemTest)
	{
		TEST_METHOD(TestFindSystemInRegistry) {
			const Jigsaw::Util::etype_info* t_info = &Jigsaw::Util::etype_info::Id<TransformSystem>();
			auto vector = Jigsaw::Systems::SystemRegistry::GetBeans();
			auto iter =std::find_if(vector.begin(), vector.end(), [=](Jigsaw::Systems::SYSTEM_REGISTRY_BEAN& bean) { return bean.type_info == t_info; });
			Assert::IsFalse(iter == vector.end());
		}

		TEST_METHOD(TestMultipleSignaturesMapToTransformSystem) {
			TransformSystem system;
			auto bean = TransformSystem::_Get_Registry_Bean();

			Jigsaw::System::SignatureBuilder builder_a, builder_b;
			builder_a.AddType(Jigsaw::Util::etype_info::Id<Transform>()).AddType(Jigsaw::Util::etype_info::Id<Jigsaw::System::UID>());
			builder_b.AddType(Jigsaw::Util::etype_info::Id<Transform>()).AddType(Jigsaw::Util::etype_info::Id<Jigsaw::Time::TimeStep>());
			Jigsaw::Unique<Jigsaw::System::Signature> sig_a = builder_a.Build();
			Jigsaw::Unique<Jigsaw::System::Signature> sig_b = builder_b.Build();

			Jigsaw::Entities::JigsawEntityClusterService cluster_service;

			Jigsaw::Entities::JigsawEntityCluster& cluster_a = cluster_service[*sig_a.get()];
			Jigsaw::Entities::JigsawEntityCluster& cluster_b = cluster_service[*sig_b.get()];

			Jigsaw::Entities::JigsawEntity entity_a_1 = cluster_a.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL);
			Jigsaw::Entities::JigsawEntity entity_a_2 = cluster_a.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL);
			Jigsaw::Entities::JigsawEntity entity_b_1 = cluster_b.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL);
			Jigsaw::Entities::JigsawEntity entity_b_2 = cluster_b.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL);

			// a conversion can be produced for each of these
			Jigsaw::Unique<Jigsaw::System::SignatureMap> conversion_map_a;
			Jigsaw::Unique<Jigsaw::System::SignatureMap> conversion_map_b;
			Assert::IsTrue(sig_a->HasConversionMap(*bean.signature.get(), &conversion_map_a));
			Assert::IsTrue(sig_b->HasConversionMap(*bean.signature.get(), &conversion_map_b));

			Assert::AreEqual(entity_a_1.GetMemberData<Transform>().position.x, (float)0);
			Assert::AreEqual(entity_a_2.GetMemberData<Transform>().position.x, (float)0);
			Assert::AreEqual(entity_b_1.GetMemberData<Transform>().position.x, (float)0);
			Assert::AreEqual(entity_b_2.GetMemberData<Transform>().position.x, (float)0);

			// conversion map needs to be passed with the entity
			system.Update(entity_a_1, *conversion_map_a);
			system.Update(entity_a_2, *conversion_map_a);
			system.Update(entity_b_1, *conversion_map_b);
			system.Update(entity_b_2, *conversion_map_b);

			// System is executed on each of the members of the system
			Assert::AreEqual(entity_a_1.GetMemberData<Transform>().position.x, (float)5);
			Assert::AreEqual(entity_a_2.GetMemberData<Transform>().position.x, (float)5);
			Assert::AreEqual(entity_b_1.GetMemberData<Transform>().position.x, (float)5);
			Assert::AreEqual(entity_b_2.GetMemberData<Transform>().position.x, (float)5);
		}
	};
}
#endif
