#ifdef _RUN_UNIT_TESTS_
#define _USE_MATH_DEFINES
#include "CppUnitTest.h"
#include <vector>
#include <memory>
#include "System/Signature.h"
#include "Util/etype_info.h"
#include "Math/LinAlg.h"
#include "Entities/JigsawEntity.h"
#include "Entities/JigsawEntityCluster.h"
#include "TestUtils.h"
#include "Ref.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Util;
using namespace Jigsaw::System;
using namespace Jigsaw::Entities;
using namespace Jigsaw;

namespace ProjectTests {
	TEST_CLASS(JigsawEntityTest)
	{
		class DestructorTest {
		public:
			DestructorTest() {}
			DestructorTest(Ref<bool> ref_bool) : ref_bool(ref_bool) {}

			Jigsaw::Ref<bool> ref_bool;
		};

		SignatureBuilder GetBuilder() {
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Vector2>());
			builder.AddType(etype_info::Id<Vector3>());
			return builder;
		}

		TEST_METHOD(TestAlterEntityMemberData) {
			SignatureBuilder builder = GetBuilder();
			Unique<Signature> u_sig = builder.Build();
			Ref<Signature> sig = MakeRef(u_sig);
			Jigsaw::Entities::JigsawEntityCluster cluster(sig);
			JigsawEntity entity = cluster.FabricateEntity(JigsawEntity::SCOPE_EPHEMERAL);
			entity.SetMemberData<Vector3>(Vector3(30.0f, 40.0f, 50.0f));
			Vector3 ret_val = entity.GetMemberData<Vector3>();

			Assert::AreEqual(30.0f, ret_val.x);
			Assert::AreEqual(40.0f, ret_val.y);
			std::default_delete<int>;
			Assert::AreEqual(50.0f, ret_val.z);
		}

		TEST_METHOD(TestDestructorCalledWhenEntityDestroyed) {
			SignatureBuilder builder;
			builder.AddType(Jigsaw::Util::etype_info::Id<DestructorTest>());
			Ref<Signature> sig = MakeRef<Signature>(builder.Build());

			Jigsaw::Entities::JigsawEntityCluster cluster(sig);
			Jigsaw::Weak<bool> weak_ref;
			bool final_destructed = false;

			Jigsaw::System::UID e_id, e_b_id;
			{
				Jigsaw::Ref<bool> strong_ref = Jigsaw::MakeRef<bool>(true);
				weak_ref = strong_ref;

				// creating two entities
				JigsawEntity entity = cluster.FabricateEntity(JigsawEntity::SCOPE_STATIC);
				entity.SetMemberData(DestructorTest(strong_ref));
				e_id = entity.GetUID();

				JigsawEntity entity_b = cluster.FabricateEntity(JigsawEntity::SCOPE_STATIC);
				entity_b.SetMemberData(DestructorTest(strong_ref));
				e_b_id = entity_b.GetUID();
			}
			Assert::IsFalse(weak_ref.expired());
			cluster.RemoveEntity(e_id);

			Assert::IsFalse(weak_ref.expired());
			cluster.RemoveEntity(e_b_id);

			Assert::IsTrue(weak_ref.expired());
		}
	};
}
#endif
