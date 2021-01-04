#ifdef _RUN_UNIT_TESTS_
#define _USE_MATH_DEFINES

#include "CppUnitTest.h"
#include "pch.h"
#include "Math/LinAlg.h"
#include <Directxmath.h>
#include "Camera.h"
#include <cmath>
#include "TestUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProjectTests {
	TEST_CLASS(CameraTest)
	{
	public:
		TEST_METHOD(TestCameraGetViewMatrix) {
			Camera cam(Vector3(1, 0, 0), Vector3(0, 1, 0));
			Mat4x4 matrix = cam.GetViewMatrix();
			Vector3 init_pos(3, 1, 0);
			Vector3 transform_pos = matrix * Vector4::AsPoint(init_pos);
			Assert::IsTrue(withinDelta(transform_pos.z, -1));
			Assert::IsTrue(withinDelta(transform_pos.x, -2));
			Assert::IsTrue(withinDelta(transform_pos.y, 0));
		};

	};
}
#endif
