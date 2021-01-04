#ifdef _RUN_UNIT_TESTS_
#ifndef _TRANSFORM_SYSTEM_MOCK_H_
#define _TRANSFORM_SYSTEM_MOCK_H_

#include "Systems/RootSystem.h"
#include "Physics/Transform.h"

namespace ProjectTests {
	JGSW_SYSTEM(TransformSystem) {
		JGSW_SYSTEM_METHOD(TransformSystemMethod, Transform);
	};
}

#endif
#endif
