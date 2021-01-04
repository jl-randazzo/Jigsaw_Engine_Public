#include "Machines/WorldEntityMachinePiece.h"

#include "Marshalling/JigsawMarshalling.h"

using namespace Jigsaw::Time;

namespace Jigsaw {
	namespace Machines {

		START_REGISTER_SERIALIZABLE_CLASS(WorldEntityMachinePiece)
		REGISTER_SERIALIZABLE_CHILD_OF(WorldEntityMachinePiece, JigsawMachinePiece)
		REGISTER_SERIALIZABLE_FIELD(WorldEntityMachinePiece, Transform, transform)
		END_REGISTER_SERIALIZABLE_CLASS(WorldEntityMachinePiece)
	}
}