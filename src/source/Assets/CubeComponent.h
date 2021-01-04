#ifndef _CUBE_COMPONENT_H_
#define _CUBE_COMPONENT_H_

#include "Ref.h"
#include "Assets/DataAssets.h"
#include "Machines/JigsawMachinePiece.h"

namespace Jigsaw {
	namespace Assets {
		struct CubeComponent {
			Jigsaw::Ref<Cube> cube_model;
		};

		JGSW_MACHINE_PIECE(CubeMachinePiece) {
		public:
			JGSW_COMPONENT_DATA(CubeComponent, cube);

		};
	}
}

#endif