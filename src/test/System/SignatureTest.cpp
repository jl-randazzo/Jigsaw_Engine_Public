#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"
#include <vector>
#include <memory>
#include "Util/etype_info.h"
#include "System/Signature.h"
#include "Util/BYTE.h"
#include "Math/LinAlg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Util;
using namespace Jigsaw::System;
using namespace Jigsaw;

namespace ProjectTests {
	TEST_CLASS(SignatureTest)
	{
		struct A {
			Quaternion quaternion;
			Vector2 vec2;
			Vector3 vec3;
		};

		struct Test_Struct {
			int a;
			int b;
		};

		struct Small {
			char a;
			char b;
		};

		struct Medium {
			float a;
			float b;
		};

		struct Large {
			double x;
		};

		struct Titan {
			double x;
		};

		Ref<Signature> signature;

		SignatureBuilder GetBuilder() {
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Vector2>());
			builder.AddType(etype_info::Id<Vector3>());
			builder.AddType(etype_info::Id<Quaternion>());
			return builder;
		}

		std::vector<etype_index> GetAltVec() {

		}

		TEST_METHOD_INITIALIZE(Setup) {
			SignatureBuilder builder = GetBuilder();
			Unique<Signature> sig = builder.Build();
			signature = MakeRef(sig);
		}

		TEST_METHOD(TestSignatureConstructionSortsTypes) {
			Assert::IsTrue(signature->GetMap().GetTypeOffset(etype_info::Id<Quaternion>()) == 0);
			Assert::IsTrue(signature->GetMap().GetTypeOffset(etype_info::Id<Vector2>()) == sizeof(Quaternion));
			Assert::IsTrue(signature->GetMap().GetTypeOffset(etype_info::Id<Vector3>()) == sizeof(Quaternion) + sizeof(Vector2));
		}

		TEST_METHOD(TestIndexFromGeneratedMap) {
			A* data = new A();
			data->vec2 = Vector2(30, 40);
			data->vec3 = Vector3(30, 40, 50);
			BYTE* bytes = CastToByteArray(data);

			const SignatureMap& map = signature->GetMap();

			void* loc_vec3 = map.GetTypeOffset(etype_info::Id<Vector3>()) + bytes;
			Vector3& vec = *static_cast<Vector3*>(loc_vec3);

			Assert::AreEqual(30.0f, vec.x);
			Assert::AreEqual(40.0f, vec.y);
			Assert::AreEqual(50.0f, vec.z);

			void* loc_vec2 = map.GetOffsetOfTypeIndex(1) + bytes;
			Vector2& vec2 = *static_cast<Vector2*>(loc_vec2);

			Assert::AreEqual(30.0f, vec2.x);
			Assert::AreEqual(40.0f, vec2.y);
		}

		TEST_METHOD(TestBuilderConstructor) {
			SignatureBuilder builder = GetBuilder();
			Unique<Signature> sig = builder.Build();
			Assert::AreEqual(sig->GetMap().GetTypeOffset(etype_info::Id <Vector2>()), (int)sizeof(Quaternion));
		}

		TEST_METHOD(TestCreateConversionMap) {
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Quaternion>()).AddType(etype_info::Id<Vector3>());

			Ref<Signature> sig = builder.Build();
			Unique<SignatureMap> cm;
			Assert::IsTrue((*signature).HasConversionMap(*sig, &cm));
			const SignatureMap& conversion_map = *cm;

			Assert::AreEqual(0, conversion_map[etype_info::Id<Quaternion>()]);
			Assert::AreEqual(0, conversion_map[0]);
			Assert::AreEqual((int)(sizeof(Quaternion) + sizeof(Vector2)), conversion_map[etype_info::Id<Vector3>()]);
			Assert::AreEqual((int)(sizeof(Quaternion) + sizeof(Vector2)), conversion_map[1]);
		}

		TEST_METHOD(TestNoValidConversion) {
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Test_Struct>()).AddType(etype_info::Id<Vector2>());

			Ref<Signature> sig = builder.Build();
			Unique<SignatureMap> conversion_map;
			Assert::IsFalse((*signature).HasConversionMap(*sig, &conversion_map));
		}

		TEST_METHOD(TestSignatureEquality) {
			auto builder_1 = GetBuilder();
			auto builder_2 = GetBuilder();
			auto sig_a = builder_1.Build();
			auto sig_b = builder_2.Build();
			Assert::IsTrue(*sig_a == *sig_b);
		}

		TEST_METHOD(TestHashInequalityAndEquality) {
			Unique<Signature> sig_a = GetBuilder().AddType(etype_info::Id<Test_Struct>()).Build();
			Unique<Signature> sig_b = GetBuilder().AddType(etype_info::Id<A>()).Build();
			std::hash<Signature> hash_;
			Assert::AreNotEqual(hash_(*sig_a), hash_(*sig_b));
			Assert::AreNotEqual(hash_(*signature), hash_(*sig_b));
			Assert::AreNotEqual(hash_(*signature), hash_(*sig_a));
			Assert::AreEqual(hash_(*sig_a), hash_(*sig_a));
		}

		TEST_METHOD(TestSignatureSizeReflectsAlignmentRequrements) {
			SignatureBuilder builder;
			Unique<Signature> sig_a = builder.AddType(etype_info::Id<Small>()).AddType(etype_info::Id<Titan>()).Build();
			Assert::AreEqual(8, sig_a->GetMap().GetTypeOffset(etype_info::Id<Titan>()));
			Assert::AreEqual(8, sig_a->GetMap()[1]);
			Assert::AreEqual(16, (int)sig_a->GetSize());
		}

		TEST_METHOD(TestEntitySizeAlignedToMaxAlignRequirements) {
			size_t a = alignof(Medium);
			SignatureBuilder builder;
			builder.AddType(etype_info::Id<Small>()).AddType(etype_info::Id<Large>()).AddType(etype_info::Id<Vector2>());
			Unique<Signature> sig_a = builder.Build();
			Assert::AreEqual(24, (int)sig_a->GetSize());

			SignatureBuilder builder_b;
			builder_b.AddType(etype_info::Id<Small>()).AddType(etype_info::Id<Medium>()).AddType(etype_info::Id<Vector2>());
			Unique<Signature> sig_b = builder_b.Build();

			Assert::AreEqual(20, (int)sig_b->GetSize());

		}
	};
}
#endif
