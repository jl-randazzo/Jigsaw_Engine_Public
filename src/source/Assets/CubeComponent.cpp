#include "CubeComponent.h"
#include "Marshalling/JigsawMarshalling.h"

using namespace Jigsaw::Machines;

namespace Jigsaw {
	namespace Assets {
		START_REGISTER_SERIALIZABLE_CLASS(CubeComponent)
		REGISTER_SERIALIZABLE_ASSET_REFERENCE(CubeComponent, Cube, cube_model)
		END_REGISTER_SERIALIZABLE_CLASS(CubeComponent)

		START_REGISTER_SERIALIZABLE_CLASS(CubeMachinePiece)
		REGISTER_SERIALIZABLE_CHILD_OF(CubeMachinePiece, JigsawMachinePiece)
		REGISTER_SERIALIZABLE_FIELD(CubeMachinePiece, CubeComponent, cube)
		END_REGISTER_SERIALIZABLE_CLASS(CubeMachinePiece)
	}
}