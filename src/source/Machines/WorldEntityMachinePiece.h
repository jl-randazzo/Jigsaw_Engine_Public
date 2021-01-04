#ifndef _WORLD_ENTITY_MACHINE_PIECE_H_
#define _WORLD_ENTITY_MACHINE_PIECE_H_

#include "JigsawMachinePiece.h"
#include "Physics/Transform.h"
#include "Time/TimeStep.h"

namespace Jigsaw {
	namespace Machines {
		/// <summary>
		/// WorldEntityMachinePiece 
		/// </summary>
		JGSW_MACHINE_PIECE(WorldEntityMachinePiece) {
		public:
			JGSW_COMPONENT_DATA(Transform, transform);
			JGSW_COMPONENT_DATA(Jigsaw::Time::TimeStep, time_step);
		};
	}
}
#endif