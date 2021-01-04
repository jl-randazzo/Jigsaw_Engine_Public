#ifdef _RUN_UNIT_TESTS_
#include "Systems/TransformSystemMock.h"

namespace ProjectTests {
	void TransformSystem::TransformSystemMethod(Transform& transform) {
		transform.position.x += 5;
	};

	REGISTER_SYSTEM(TransformSystem);
}
#endif