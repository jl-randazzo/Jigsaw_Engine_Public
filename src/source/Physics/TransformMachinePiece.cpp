#include "Physics/TransformMachinePiece.h"
#include "Math/LinAlg.h"
#include "Physics/Transform.h"
#include "Marshalling/JigsawMarshalling.h"


using namespace Jigsaw::Machines;
namespace Jigsaw {
	namespace Physics {
		START_REGISTER_SERIALIZABLE_CLASS(TransformMachinePiece);
		REGISTER_SERIALIZABLE_CHILD_OF(TransformMachinePiece, JigsawMachinePiece);
		REGISTER_SERIALIZABLE_FIELD(TransformMachinePiece, Transform, transform);
		END_REGISTER_SERIALIZABLE_CLASS(TransformMachinePiece);
	}
}
