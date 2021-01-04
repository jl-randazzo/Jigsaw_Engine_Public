#include "RootSystem.h"

#include "Physics/Transform.h"
#include "Time/TimeStep.h"
#include "Assets/CubeComponent.h"

namespace Jigsaw {
	namespace Systems {
#define RAD_S .2f
#define ANGLE Vector3(1, 1, 0)

		JGSW_SYSTEM(CubeSystem_O) {
		public:
			JGSW_SYSTEM_METHOD(MoveCube, Transform, const Jigsaw::Time::TimeStep, Jigsaw::Assets::CubeComponent);

		};

		JGSW_SYSTEM(CubeSystem) {
		public:
			JGSW_SYSTEM_METHOD(RotateCube, Transform, const Jigsaw::Time::TimeStep, Jigsaw::Assets::CubeComponent);

		};
	}
}