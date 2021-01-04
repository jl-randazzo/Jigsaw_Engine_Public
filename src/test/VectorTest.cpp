#ifdef _RUN_UNIT_TESTS_
#define _USE_MATH_DEFINES

#include "CppUnitTest.h"
#include "pch.h"
#include "Math/LinAlg.h"
#include "Marshalling/JigsawMarshalling.h"
#include <Directxmath.h>
#include <iostream>
#include "TestUtils.h"
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std::chrono;

namespace ProjectTests {


	TEST_CLASS(LinAlgTest)
	{
	public:

		TEST_METHOD(testDot)
		{
			Vector3 vec(1, 2, 1);
			Vector3 vec_2(1, 2, 1);
			Assert::IsTrue(Vector3(1,2,1) == Vector3(1,2,1));
			Assert::IsFalse(vec != vec_2);
			float Dot = Vector3(1, 2, 1).Dot(Vector3(1, 1.5, 2));
			Assert::IsTrue(withinDelta(Dot, 6));
			float Dot_2 = Vector2(6, 7.5).Dot(Vector2(.5, 2));
			Assert::IsTrue(withinDelta(Dot_2, 18));

			float Dot_3 = Vector3(7, 7, 0).Dot(Vector2(7, 7));

			Assert::AreEqual(Dot_3, (float)98.0);
		}

		TEST_METHOD(testXMFLOAT3Conversion)
		{
			DirectX::XMFLOAT3 f_3 = Vector3(1, 2, 7);
			Assert::AreEqual(1.0f, f_3.x);
			Assert::AreEqual(2.0f, f_3.y);
			Assert::AreEqual(7.0f, f_3.z);
		}

		TEST_METHOD(testCrossProduct) {
			float root_14 = sqrt(14);
			Vector3 view(Vector3(1 / root_14, 2 / root_14, 3 / root_14));
			Vector3 tangent(view.Cross(Vector3::Z()));
			Vector3 normtan(tangent.Normalized());
			Vector3 bitangent(normtan.Cross(view).Normalized());

			float length = tangent.GetLength();
			float bi_length = bitangent.GetLength();
			Assert::AreEqual(bi_length, 1.f);

			Assert::IsTrue(withinDelta(view.Dot(normtan), 0));
			Assert::IsTrue(withinDelta(view.Dot(bitangent), 0));
			Assert::IsTrue(withinDelta(normtan.Dot(bitangent), 0));
		}

		TEST_METHOD(testAssignment) {
			Vector3 assign = Vector3(1, 1, 1);
			Assert::IsTrue(assign == Vector3(1, 1, 1));
			assign.x = 3.0f;
			Assert::AreEqual(3.0f, assign.x);
		}

		TEST_METHOD(testRotation) {
			Quaternion q(Vector3::Z(), M_PI_4);
			Vector3 p(1, 0, 0);
			Vector3 result = q.Rotate(p);
			Assert::IsTrue(withinDelta(1, result.GetLength()));
			Assert::IsTrue(withinDelta(cosf(M_PI_4), result.x));
			Assert::IsTrue(withinDelta(sinf(M_PI_4), result.y));
			Assert::IsTrue(withinDelta(0, result.z));
		}

		TEST_METHOD(testMatrixAssignment) {
			Mat4x4 mat;
			Assert::IsTrue(0 == mat(0, 3));
			mat(0, 3) = 4.0f;
			float f = mat(0, 3);
			Assert::AreEqual(f, 4.0f);
		}

		TEST_METHOD(testMatrixAssignmentFromVector4) {
			Vector4 m0(1, 0, 1, 7);
			Vector4 m1(1, 0, 1, 0);
			Vector4 m2(1, 0, 1, 3);
			Vector4 m3(1, 0, 1, 0);
			Mat4x4 mat1(m0, m1, m2, m3);

			Assert::IsTrue(mat1(0, 0) == 1 && mat1(0, 3) == 7 && mat1(2, 3) == 3);
			
			Mat4x4 mat;
			mat = mat1;
			Assert::IsTrue(mat(0, 0) == 1 && mat(0, 3) == 7 && mat(2, 3) == 3);

			Mat4x4 transpose = mat.Transpose();
			Assert::IsTrue(transpose(0, 0) == 1 && transpose(3, 0) == 7 && transpose(3, 2) == 3);

			return;
		}

		TEST_METHOD(testVec4Mat4x4Multiply) {
			Vector4 m0(1, 0, 1, 7);
			Vector4 m1(1, 0, 1, 0);
			Vector4 m2(1, 0, 1, 3);
			Vector4 m3(1, 0, 1, 0);
			Mat4x4 mat(m0, m1, m2, m3);

			Vector4 v(2, 3, 4, 7);

			Vector4 res_1 = v * mat;
			Vector4 res_2 = mat * v;

			Assert::IsTrue(res_1 == Vector4(16, 0, 16, 26));
			Assert::IsTrue(res_2 == Vector4(55, 6, 27, 6));
		}

		TEST_METHOD(testMat4Multiplication) {
			Vector4 m0(1, 0, 1, 7);
			Vector4 m1(1, 0, 1, 0);
			Vector4 m2(1, 0, 1, 3);
			Vector4 m3(1, 0, 1, 0);
			Mat4x4 mat(m0, m1, m2, m3);

			m0 = Vector4(0, 2, 1, 3);
			m1 = Vector4(1, 1, 1, 1);
			m2 = Vector4(2, 1, 0, 4);
			m3 = Vector4(0, 0, 1, 2);
			Mat4x4 other(m0, m1, m2, m3);

			Mat4x4 res_1 = mat * other;
			Mat4x4 res_2 = other * mat;

			// checking first result
			Assert::IsTrue(res_1.GetRow(0) == Vector4(2, 3, 8, 21));
			Assert::IsTrue(res_1.GetRow(1) == Vector4(2, 3, 1, 7));
			Assert::IsTrue(res_1.GetRow(2) == Vector4(2, 3, 4, 13));
			Assert::IsTrue(res_1.GetRow(3) == Vector4(2, 3, 1, 7));
			
			// checking second result
			Assert::IsTrue(res_2.GetRow(0) == Vector4(6, 0, 6, 3));
			Assert::IsTrue(res_2.GetRow(1) == Vector4(4, 0, 4, 10));
			Assert::IsTrue(res_2.GetRow(2) == Vector4(7, 0, 7, 14));
			Assert::IsTrue(res_2.GetRow(3) == Vector4(3, 0, 3, 3));
		}
	};
}
#endif