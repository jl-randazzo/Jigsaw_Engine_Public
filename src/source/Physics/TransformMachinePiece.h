#ifndef _TRANSFORM_MACHINE_PIECE_H_
#define _TRANSFORM_MACHINE_PIECE_H_

#include "Machines/JigsawMachinePiece.h"
#include "Physics/Transform.h"

namespace Jigsaw {
	namespace Physics {
		JGSW_MACHINE_PIECE(TransformMachinePiece) {
		public:
			TransformMachinePiece() {}

			JGSW_COMPONENT_DATA(Transform, transform);

		};
	}
}
#endif