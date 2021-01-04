#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"
#include "Entities/JigsawEntity.h"
#include "Entities/JigsawEntityCluster.h"
#include "Util/etype_info.h"
#include "System/Signature.h"
#include <vector>
#include "Math/LinAlg.h"
#include "TestUtils.h"

using namespace Jigsaw;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Util;
using namespace Jigsaw::System;
using namespace Jigsaw::Entities;

namespace ProjectTests {
	TEST_CLASS(JigsawEntityClusterTest)
	{
		SignatureBuilder GetBuilder() {
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Vector2>());
			builder.AddType(etype_info::Id<Vector3>());
			return builder;
		}

		Ref<JigsawEntityCluster> cluster_root;
		Ref<Signature> signature;

		TEST_METHOD_INITIALIZE(Setup) {
			SignatureBuilder builder = GetBuilder();
			Unique<Signature> sig = builder.Build();
			signature = MakeRef(sig);
			cluster_root = MakeRef<JigsawEntityCluster>(signature);
		}

		TEST_METHOD(TestEntityClusterPackAndFetch) {
			Assert::AreEqual(0, (int)cluster_root->GetClusterCount());
			JigsawEntity entity = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);
			Assert::AreEqual((int)JigsawEntity::SCOPE_PERSISTENT, (int)entity.GetScope());
			entity.SetMemberData<Vector2>(Vector2(0, 14));

			JigsawEntity fetched_entity = cluster_root->FetchEntity(entity.GetUID());
			Assert::AreEqual((int)JigsawEntity::SCOPE_PERSISTENT, (int)fetched_entity.GetScope());
			Vector2 member_vec = fetched_entity.GetMemberData<Vector2>();
			Assert::IsTrue(withinDelta(0, member_vec.x));
			Assert::IsTrue(withinDelta(14, member_vec.y));
		}

		TEST_METHOD(TestEntityClusterRootChangesRegisterInEntity) {
			JigsawEntity entity_a = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);
			JigsawEntity entity_b = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);

			entity_a.SetMemberData<Vector2>(Vector2(0, 0));
			entity_b.SetMemberData<Vector2>(Vector2(7, 8));

			JigsawEntity entity_a_fetch_1 = cluster_root->FetchEntity(entity_a.GetUID());
			// Alter member data
			entity_a_fetch_1.SetMemberData<Vector2>(Vector2(-1, 700));

			// and fetch again
			JigsawEntity entity_a_fetch_2 = cluster_root->FetchEntity(entity_a.GetUID());
			// check that changes processed
			Assert::IsTrue(withinDelta(entity_a_fetch_2.GetMemberData<Vector2>().x, -1));
			Assert::IsTrue(withinDelta(entity_a_fetch_2.GetMemberData<Vector2>().y, 700));

			JigsawEntity entity_b_fetch_1 = cluster_root->FetchEntity(entity_b.GetUID());
			Assert::IsTrue(withinDelta(entity_b_fetch_1.GetMemberData<Vector2>().x, 7));
			Assert::IsTrue(withinDelta(entity_b_fetch_1.GetMemberData<Vector2>().y, 8));
		}

		void BatchEntityAdd(int count) {
			for (int i = 0; i < count; i++) {
				JigsawEntity entity = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);
			}
		}

		TEST_METHOD(TestEntityClusterRootClusterCountExpands) {
			for (int i = 1; i <= 5; i++) {
				BatchEntityAdd(CLUSTER_SIZE);
				Assert::AreEqual(i, (int)cluster_root->GetClusterCount());
			}
		}

		TEST_METHOD(TestEntityClusterRemovalShrinksNodes) {
			BatchEntityAdd(CLUSTER_SIZE - 1);
			JigsawEntity entity_a = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);
			JigsawEntity entity_b = cluster_root->FabricateEntity(JigsawEntity::SCOPE_PERSISTENT);

			entity_b.SetMemberData<Vector2>(Vector2(12, -3));

			Assert::AreEqual(2, (int)cluster_root->GetClusterCount());
			cluster_root->RemoveEntity(entity_a.GetUID());
			Assert::AreEqual(1, (int)cluster_root->GetClusterCount());

			JigsawEntity fetch_entity_b = cluster_root->FetchEntity(entity_b.GetUID());
			Assert::IsTrue(withinDelta(fetch_entity_b.GetMemberData<Vector2>().x, 12));
			Assert::IsTrue(withinDelta(fetch_entity_b.GetMemberData<Vector2>().y, -3));
		}

	};
}
#endif
