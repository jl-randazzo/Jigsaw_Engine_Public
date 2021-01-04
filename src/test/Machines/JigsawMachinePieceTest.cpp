#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Entities/JigsawEntityCluster.h"
#include "Machines/JigsawMachinePiece.h"
#include "Physics/Transform.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProjectTests {
	JGSW_MACHINE_PIECE(MockJigsawMachinePiece) {
	public:
		MockJigsawMachinePiece() {}
	
		JGSW_COMPONENT_DATA(Transform, transform)

	};

	TEST_CLASS(JigsawMachinePieceTest)
	{
		TEST_METHOD(TestJigsawMachinePieceMacroComposesSignature) {
			MockJigsawMachinePiece piece;
			Jigsaw::System::SignatureBuilder builder;
			piece.ComposeSignature(builder);
			auto _sig = builder.Build();
			const Jigsaw::System::Signature& signature = *_sig.get();;
			Jigsaw::Util::etype_index t_index(Jigsaw::Util::etype_info::Id<Transform>());
			Assert::IsTrue(signature.GetAlignedTypes().at(0) == t_index);
		}
		
		TEST_METHOD(TestFabricatedEntityInstantiatedWithDefaultMachineValues) {
			MockJigsawMachinePiece piece;
			piece.transform.position = Vector3(1, 2, 3);
			Jigsaw::System::SignatureBuilder builder;
			piece.ComposeSignature(builder);
			auto signature = Jigsaw::Ref<Jigsaw::System::Signature>(std::move(builder.Build()));

			Jigsaw::Entities::JigsawEntityCluster cluster(signature);
			Jigsaw::Entities::JigsawEntity entity(cluster.FabricateEntity(Jigsaw::Entities::JigsawEntity::SCOPE_EPHEMERAL));
			piece.Turn(entity);

			Transform result = entity.GetMemberData<Transform>();
			Assert::IsTrue(result.position == piece.transform.position);
		}
	};
}
#endif
