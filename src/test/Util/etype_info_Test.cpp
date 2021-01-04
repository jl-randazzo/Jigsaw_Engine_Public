#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"
#include "Util/etype_info.h"
#include "Math/LinAlg.h"
#include <map>
#include<unordered_map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Util;

namespace ProjectTests {
	TEST_CLASS(etype_info_Test)
	{
		// Assert that the 'Id' call returns an etype_info whos underlying type_info struct contains the same data
		TEST_METHOD(Test_etype_info_Id) {
			const etype_info& et_i = etype_info::Id<Vector2>();
			Assert::AreEqual(et_i.size, sizeof(Vector2));
			Assert::AreEqual(et_i.GetBase().name(), typeid(Vector2).name());
			Assert::AreEqual(et_i.GetBase().hash_code(), typeid(Vector2).hash_code());
		}

		// Assert that the 'Id' call will return the same reference every time
		TEST_METHOD(Test_etype_info_IdReturnsSame) {
			const etype_info* et_i_ref = &etype_info::Id<Vector2>();
			const etype_info* et_i_ref_2 = &etype_info::Id<Vector2>();
			Assert::IsTrue(et_i_ref == et_i_ref_2);
		}

		template <typename Map>
		void _TestMap(Map map) {
			map.insert(std::make_pair(etype_index(etype_info::Id<Vector2>()), 1));
			map.insert(std::make_pair(etype_index(etype_info::Id<Vector3>()), 2));
			map.insert(std::make_pair(etype_index(etype_info::Id<Mat4x4>()), 3));

			Assert::AreEqual(map.at(etype_index(etype_info::Id<Mat4x4>())), 3);
			Assert::AreEqual(map.at(etype_index(etype_info::Id<Vector3>())), 2);
			Assert::AreEqual(map.at(etype_index(etype_info::Id<Vector2>())), 1);

			Assert::IsTrue(map.find(etype_index(etype_info::Id<Quaternion>())) == map.end());
		}

		TEST_METHOD(Test_etype_info_UsedAsMapIndex) {
			std::map<etype_index, int> map;
			_TestMap<std::map<etype_index, int>>(map);
		}

		TEST_METHOD(Test_etype_info_UnorderedMapIndex) {
			std::unordered_map<etype_index, int> map;
			_TestMap<std::unordered_map<etype_index, int>>(map);
		}
	};
}
#endif
