#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"
#include "Entities/JigsawEntityClusterService.h"
#include "System/Signature.h"
#include "Math/LinAlg.h"
#include "TestUtils.h"

using namespace Jigsaw;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Entities;
using namespace Jigsaw::System;
using namespace Jigsaw::Util;

namespace ProjectTests {
	TEST_CLASS(JigsawEntityClusterServiceTest)
	{
		class MockConstruction : public JigsawEntityClusterService {
		public:
			MockConstruction() : JigsawEntityClusterService() {}
		};

		TEST_METHOD_INITIALIZE(Setup) {
			cluster_service = static_cast<JigsawEntityClusterService*>(new MockConstruction());
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Vector2>());
			builder.AddType(etype_info::Id<Vector3>());
			Unique<Signature> u_sig = builder.Build();
			signature = MakeRef(u_sig);
		}

		JigsawEntityClusterService* cluster_service;
		Ref<Signature> signature;

		TEST_METHOD(TestFetchClusterForSignature) {
			JigsawEntityCluster& root = (*cluster_service)[*signature];
			Assert::IsTrue(root.GetSignature() == *signature);
		}

		TEST_METHOD(TestAddEntityToClusterAndFetch) {
			JigsawEntityCluster& root = (*cluster_service)[*signature];
			JigsawEntity entity = root.FabricateEntity(JigsawEntity::SCOPE_EPHEMERAL);
			entity.SetMemberData<Vector2>(Vector2(-3, 7));

			JigsawEntityCluster& root_alt = (*cluster_service)[*signature];
			JigsawEntity fetched_entity = root_alt.FetchEntity(entity.GetUID());
			Assert::IsTrue(withinDelta(fetched_entity.GetMemberData<Vector2>().x, -3));
		}
	};
}
#endif
