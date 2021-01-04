#ifdef _RUN_UNIT_TESTS_

#include "File_IO/FileUtil.h"
#include "CppUnitTest.h"
#include "Marshalling/MarshallingUtil.h"
#include "Assets/ASSET_LOAD_RESULT.h"
#include "Assets/DataAssets.h"
#include "TestUtils.h"
#include "Ref.h"
#include "Machines/WorldEntityMachinePiece.h"
#include "Machines/JigsawMachine.h"
#include "Application/ApplicationRootProperties.h"
#include "Assets/CubeComponent.h"
#include <iostream>
#include <fstream>

using namespace Jigsaw;
using namespace Jigsaw::Assets;
using namespace Jigsaw::Marshalling;
using namespace Jigsaw::System;
using namespace Jigsaw::Machines;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ProjectTests {

	TEST_CLASS(JigsawMarshallingTest) {

		TEST_METHOD(TestRetrieveMarshallMapTypeNames) {
			const Jigsaw::Util::etype_info& e_inf = Jigsaw::Util::etype_info::Id<t_field_class>();
			const MarshallingMap& map = MarshallingRegistry::GetMarshallingMap(e_inf);
			const field_data* val_field_data = map.GetFieldDataByName("val");

			const Jigsaw::Util::etype_info& inf = val_field_data->GetType();
			const type_info& t_info = inf.GetBase();
			Unique<int> u;

			Assert::AreEqual(t_info.name(), std::string("int").c_str());
		}

		TEST_METHOD(TestSetValuesUsingFieldData) {
			const Jigsaw::Util::etype_info& e_inf = Jigsaw::Util::etype_info::Id<t_field_class>();
			const MarshallingMap& map = MarshallingRegistry::GetMarshallingMap(e_inf);
			const field_data* val_field_data = map.GetFieldDataByName("val");

			int x = 5;
			t_field_class clazz;

			val_field_data->setter(&clazz, &x);
			const int y = *static_cast<const int*>(val_field_data->getter(&clazz));

			Assert::AreEqual(x, y);
		}

		TEST_METHOD(TestUnmarshalJsonToObject) {
			const char* file_name = "./Assets/json_test.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::AreEqual(320, unmarshalled->val);
			Assert::AreEqual("value", unmarshalled->x.c_str());
		}

		TEST_METHOD(TestUnmarshalNestedObjectJsonToRef) {
			const char* file_name = "./Assets/json_test_a.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::AreEqual(-320, unmarshalled->val);
			Assert::AreEqual("value_we_need_#&*(&@#$^(", unmarshalled->x.c_str());
			t_uid id{ 83823740234070, 1087018347001 };
			Assert::IsTrue(id == unmarshalled->id);
		}

		TEST_METHOD(TestUnmarshalStringArray) {
			const char* file_name = "./Assets/json_test_b.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::AreEqual("value_1", unmarshalled->str_array[0].c_str());
			Assert::AreEqual("value_2", unmarshalled->str_array[1].c_str());
			Assert::AreEqual("value_3", unmarshalled->str_array[2].c_str());
		}

		TEST_METHOD(TestUnmarshalFloatingPointNumbers) {
			const char* file_name = "./Assets/json_test_c.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::AreEqual(-3.278, unmarshalled->d_precision);
			Assert::AreEqual(.5f, unmarshalled->s_precision);
		}

		TEST_METHOD(TestUnmarshalNestedArrayTypes) {
			bool c_a = std::is_copy_assignable<t_uid_class>::value;
			bool c_c = std::is_copy_constructible<t_uid_class>::value;
			bool m_a = std::is_move_assignable<t_uid_class>::value;
			const char* file_name = "./Assets/json_test_d.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			t_uid id(740183, 387892);
			t_uid id_o(7829374, 0);
			t_uid empty(0, 0);
			t_uid_class& second = unmarshalled->t_uids[1];
			Assert::IsTrue(id == unmarshalled->t_uids[0].ids[0]);
			Assert::IsTrue(id_o == unmarshalled->t_uids[0].ids[1]);
			Assert::IsTrue(id_o == unmarshalled->t_uids[1].ids[0]);
			Assert::IsTrue(empty == second.ids[1]);
			Assert::AreEqual(std::string("blah").c_str(), unmarshalled->t_uids[0].uid_name.c_str());
		}

		TEST_METHOD(TestUnmarshalEscapeCharactersAndBooleans) {
			const char* file_name = "./Assets/json_test_e.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::IsTrue(std::string("\"somewhere far from here \n\t\"") == unmarshalled->x);
			Assert::IsTrue(unmarshalled->truth_a);
			Assert::IsFalse(unmarshalled->truth_b);
		}

		TEST_METHOD(TestUnmarshalInheritedType) {
			const char* file_name = "./Assets/json_test_f.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_uid_parclass> unmarshalled = MarshallingUtil::Unmarshal<t_uid_parclass>(f_data);
			Jigsaw::Ref<t_uid_class> inherited = std::dynamic_pointer_cast<t_uid_class>(unmarshalled);
			t_uid expected(89827348, 1298149);
			Assert::IsTrue(inherited.get());
			Assert::AreEqual("the main uid", inherited->uid_name.c_str());
			Assert::IsTrue(expected == inherited->main_uid);
		}

		TEST_METHOD(TestUnmarshalIgnoresNonExistentProperties) {
			const char* file_name = "./Assets/json_test_g.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_uid_parclass> unmarshalled = MarshallingUtil::Unmarshal<t_uid_parclass>(f_data);
			Jigsaw::Ref<t_uid_child_child> inherited = std::dynamic_pointer_cast<t_uid_child_child>(unmarshalled);
			t_uid expected(89827348, 1298149);
			Assert::IsTrue(inherited.get());
			Assert::AreEqual("the main uid", inherited->uid_name.c_str());
			Assert::IsTrue(expected == inherited->main_uid);
			Assert::AreEqual("super child name found", inherited->super_child.c_str());
		}

		long long read_data_a = -2430009748240096423;
		long long read_data_b = -6444701580585300038;
		TEST_METHOD(TestUnmarshalUsesProducerMethod) {
			const char* file_name = "./Assets/json_test_h.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_uid_parclass> unmarshalled = MarshallingUtil::Unmarshal<t_uid_parclass>(f_data);
			Jigsaw::Ref<t_uid_child_child> inherited = std::dynamic_pointer_cast<t_uid_child_child>(unmarshalled);
			Assert::IsTrue(inherited.get());
			Assert::IsTrue(Jigsaw::System::UID(read_data_a, read_data_b) == inherited->uid);
		}

		TEST_METHOD(TestReturnSerializableAssetReferencesAndResolveThroughInjectionChain) {
			const char* file_name = "./Assets/json_test_i.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			std::istringstream json_stream(Jigsaw::File_IO::GetStringStreamFromFile(f_data));
			Jigsaw::Marshalling::JSONNodeReader node(json_stream, MarshallingRegistry::GetMarshallingMap(Jigsaw::Util::etype_info::Id<t_field_class>()));
			UNMARSHALLING_RESULT res = node.BuildNode();
			t_field_class* obj = static_cast<t_field_class*>(res.raw_data);

			Assert::IsFalse(res.unresolved_references.empty());
			UNRESOLVED_REFERENCE unresolved_reference = res.unresolved_references.at(0);
			Assert::IsTrue(read_data_a == unresolved_reference.ref_id.GetData_a());
			Assert::IsTrue(read_data_b == unresolved_reference.ref_id.GetData_b());

			// leveraging the injection chain to set the value of missing references
			Jigsaw::Assets::AssetDescriptor descriptor;
			descriptor.id = unresolved_reference.ref_id;
			Ref<Jigsaw::Assets::DataAsset> resource = Ref<Jigsaw::Assets::DataAsset>(new Jigsaw::Assets::ShaderResource(descriptor));
			unresolved_reference.injector(&resource);

			unresolved_reference = res.unresolved_references.at(1);
			Ref<Jigsaw::Assets::DataAsset> resource_o = Ref<Jigsaw::Assets::DataAsset>(new Jigsaw::Assets::ShaderResource(descriptor));
			unresolved_reference.injector(&resource_o);
			obj->t_uids.push_back(t_uid_class());

			Assert::IsTrue(resource.get() == obj->t_uids[0].asset_ref.get());
			Assert::IsTrue(resource_o.get() == obj->t_uids[1].asset_ref.get());
		}

		TEST_METHOD(TestContextRootedInitialization) {
			const char* file_name = "./Assets/json_test_j.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_uid_class> unmarshalled = MarshallingUtil::Unmarshal<t_uid_class>(f_data);
			Assert::IsTrue(unmarshalled->root.GetTUIDClass() == unmarshalled.get());
		}

		TEST_METHOD(TestUniquePointerCast) {
			const char* file_name = "./Assets/json_test_k.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_uid_child_child> unmarshalled = std::dynamic_pointer_cast<t_uid_child_child>(MarshallingUtil::Unmarshal<t_uid_parclass>(f_data));
			t_uid_c* casted_pointer = dynamic_cast<t_uid_c*>(unmarshalled->pointer_object.get());
			Assert::IsTrue(casted_pointer);
		}

		TEST_METHOD(TestArrayOfPointersCast) {
			const char* file_name = "./Assets/json_test_l.json";
			Jigsaw::File_IO::FILE_DATA f_data = Jigsaw::File_IO::GetResourceFile(file_name);
			Jigsaw::Ref<t_field_class> unmarshalled = MarshallingUtil::Unmarshal<t_field_class>(f_data);
			Assert::IsTrue(unmarshalled->truth_b);
			Assert::AreEqual(33333, unmarshalled->val);
			Assert::AreEqual("string_0", unmarshalled->str_array[0].c_str());
			Assert::AreEqual("string_1", unmarshalled->str_array[1].c_str());

			t_uid** t_uids = unmarshalled->pointer_array.data();
			t_uid_c* casted_0 = dynamic_cast<t_uid_c*>(t_uids[0]);
			t_uid_c* casted_2 = dynamic_cast<t_uid_c*>(t_uids[2]);
			Assert::IsNotNull(casted_0);
			Assert::IsNotNull(casted_2);
		}

		TEST_METHOD(TestMarshallPrimitiveTypes) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::primitive_type_wrapper\",\r\n\t\"d_precision\":-0.233847,\r\n\t\"long_val\":823094879349238402,\r\n\t\"s_precision\":0.05,\r\n\t\"str\":\"Series of characters\\n\",\r\n\t\"truth\":true,\r\n\t\"val\":-283\r\n}";
			primitive_type_wrapper wrapper;
			wrapper.val = -283;
			wrapper.truth = true;
			wrapper.long_val = 823094879349238402;
			wrapper.d_precision = -0.233847;
			wrapper.s_precision = 0.05;
			wrapper.str = "Series of characters\n";

			std::string marshalled = MarshallingUtil::Marshal<primitive_type_wrapper>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());
		}

		TEST_METHOD(TestMarshallArray) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::t_vector_wrapper\",\r\n\t\"str_vector\":[\r\n\t\t\"String_1\",\r\n\t\t\"String_2\",\r\n\t\t\"String_3\"\r\n\t]\r\n}";
			t_vector_wrapper wrapper;
			wrapper.str_vector.push_back("String_1");
			wrapper.str_vector.push_back("String_2");
			wrapper.str_vector.push_back("String_3");

			std::string marshalled = MarshallingUtil::Marshal<t_vector_wrapper>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());

			const char* expected_b = "{\r\n\t\"@type\":\"ProjectTests::t_vector_wrapper\",\r\n\t\"int_vector\":[\r\n\t\t2342,\r\n\t\t\-222,\r\n\t\t0,\r\n\t\t100000000,\r\n\t\t0\r\n\t]\r\n}";
			t_vector_wrapper wrapper_b;
			wrapper_b.int_vector.push_back(2342);
			wrapper_b.int_vector.push_back(-222);
			wrapper_b.int_vector.push_back(0);
			wrapper_b.int_vector.push_back(100000000);
			wrapper_b.int_vector.push_back(0);

			std::string marshalled_b = MarshallingUtil::Marshal<t_vector_wrapper>(&wrapper_b);
			Assert::AreEqual(expected_b, marshalled_b.c_str());
		}

		TEST_METHOD(TestMarshallAndUnmarshallObjectArray) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::t_vector_wrapper\",\r\n\t\"t_uid_vector\":[\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":1111,\r\n\t\t\t\"data_b\":-1111\r\n\t\t},\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":9876,\r\n\t\t\t\"data_b\":-1\r\n\t\t}\r\n\t]\r\n}";
				
			t_vector_wrapper wrapper;
			t_uid t_u, t_u_b;
			t_u.data_a = 1111;
			t_u.data_b = -1111;
			t_u_b.data_a = 9876;
			t_u_b.data_b = -1;

			wrapper.t_uid_vector.push_back(t_u);
			wrapper.t_uid_vector.push_back(t_u_b);

			std::string marshalled = MarshallingUtil::Marshal<t_vector_wrapper>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());

			std::istringstream marshalling_stream(marshalled);
			JSONNodeReader reader(marshalling_stream, MarshallingRegistry::GetMarshallingMap(Jigsaw::Util::etype_info::Id<t_vector_wrapper>()));
			UNMARSHALLING_RESULT result = reader.BuildNode();

			t_vector_wrapper* unmarshalled = static_cast<t_vector_wrapper*>(result.raw_data);
			Assert::IsTrue(unmarshalled->t_uid_vector.at(0) == t_u);
			Assert::IsTrue(unmarshalled->t_uid_vector.at(1) == t_u_b);
		}

		TEST_METHOD(TestMarshallUniquePointersAndPointerArrays) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::t_ptr_class\",\r\n\t\"ptr_array\":[\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid_c\",\r\n\t\t\t\"data_c\":10000,\r\n\t\t\t\"data_a\":0,\r\n\t\t\t\"data_b\":-121\r\n\t\t},\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":10,\r\n\t\t\t\"data_b\":9\r\n\t\t}\r\n\t],\r\n\t\"t_u_c\":{\r\n\t\t\"@type\":\"ProjectTests::t_uid_c\",\r\n\t\t\"data_c\":0,\r\n\t\t\"name\":\"named_t_u_c\",\r\n\t\t\"data_a\":-11,\r\n\t\t\"data_b\":-8\r\n\t}\r\n}";

			t_ptr_class wrapper;
			wrapper.ptr_array.push_back(new t_uid_c(0, -121, 10000));
			wrapper.ptr_array.push_back(new t_uid(10, 9));
			t_uid_c* t_u_c = new t_uid_c(-11, -8, 0);
			t_u_c->name = Jigsaw::Unique<std::string>(new std::string("named_t_u_c"));
			auto unique = Jigsaw::Unique<t_uid_c>(t_u_c);
			wrapper.t_u_c = std::move(unique);

			std::string marshalled = MarshallingUtil::Marshal<t_ptr_class>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());
		}

		TEST_METHOD(TestMarshallReferenceAndBoundedArray) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::t_ref_bound\",\r\n\t\"asset_ref\":\"87E43B02-AD4B-4BC7-A7B3-94CE39EAC0AE\",\r\n\t\"t_uids\":[\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":0,\r\n\t\t\t\"data_b\":0\r\n\t\t},\r\n\t\t{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":1111,\r\n\t\t\t\"data_b\":1233\r\n\t\t}\r\n\t]\r\n}";

			t_ref_bound wrapper;
			wrapper.t_uids[1].data_a = 1111;
			wrapper.t_uids[1].data_b = 1233;

			AssetDescriptor descriptor;
			std::string* id = new std::string("87e43b02-ad4b-4bc7-a7b3-94ce39eac0ae");
			Jigsaw::System::UIDFromString(&descriptor.id, id);

			Ref<Jigsaw::Assets::ShaderResource> resource = Ref<Jigsaw::Assets::ShaderResource>(new Jigsaw::Assets::ShaderResource(descriptor));
			wrapper.asset_ref = resource;

			std::string marshalled = MarshallingUtil::Marshal<t_ref_bound>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());
		}

		TEST_METHOD(TestMarshallSerializableRefAndInlineObject) {
			const char* expected = "{\r\n\t\"@type\":\"ProjectTests::t_class_wrapper\",\r\n\t\"s_ref\":\"87E43B02-AD4B-4BC7-A7B3-94CE39EAC0AE\",\r\n\t\"t_f_c\":{\r\n\t\t\"@type\":\"ProjectTests::t_field_class\",\r\n\t\t\"d_precision\":0,\r\n\t\t\"id\":{\r\n\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\"data_a\":0,\r\n\t\t\t\"data_b\":0\r\n\t\t},\r\n\t\t\"s_precision\":0,\r\n\t\t\"str_array\":[\r\n\t\t\t\"test_string\"\r\n\t\t],\r\n\t\t\"t_uids\":[\r\n\t\t\t{\r\n\t\t\t\t\"@type\":\"ProjectTests::t_uid_class\",\r\n\t\t\t\t\"ids\":[\r\n\t\t\t\t\t{\r\n\t\t\t\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\t\t\t\"data_a\":0,\r\n\t\t\t\t\t\t\"data_b\":0\r\n\t\t\t\t\t},\r\n\t\t\t\t\t{\r\n\t\t\t\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\t\t\t\"data_a\":0,\r\n\t\t\t\t\t\t\"data_b\":0\r\n\t\t\t\t\t}\r\n\t\t\t\t],\r\n\t\t\t\t\"root\":{\r\n\t\t\t\t\t\"@type\":\"ProjectTests::t_context_root\"\r\n\t\t\t\t},\r\n\t\t\t\t\"uid_name\":\"\",\r\n\t\t\t\t\"main_uid\":{\r\n\t\t\t\t\t\"@type\":\"ProjectTests::t_uid\",\r\n\t\t\t\t\t\"data_a\":0,\r\n\t\t\t\t\t\"data_b\":0\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t],\r\n\t\t\"truth_a\":false,\r\n\t\t\"truth_b\":false,\r\n\t\t\"val\":0,\r\n\t\t\"x\":\"\"\r\n\t}\r\n}";
			t_class_wrapper wrapper;

			AssetDescriptor descriptor;
			descriptor.fully_qualified_type_name = "ProjectTests::t_uid";
			std::string* id = new std::string("87e43b02-ad4b-4bc7-a7b3-94ce39eac0ae");
			Jigsaw::System::UIDFromString(&descriptor.id, id);
			Ref<Jigsaw::Assets::JsonAsset> json_ref = MakeRef<Jigsaw::Assets::JsonAsset>(descriptor);
			wrapper.s_ref = Jigsaw::Assets::SerializedRef<t_uid>(json_ref);
			t_uid_class t_u;
			wrapper.t_f_c.t_uids.push_back(t_u);
			wrapper.t_f_c.str_array.push_back("test_string");

			std::string marshalled = MarshallingUtil::Marshal<t_class_wrapper>(&wrapper);
			Assert::AreEqual(expected, marshalled.c_str());

			std::istringstream marshalling_stream(marshalled);
			JSONNodeReader reader(marshalling_stream, MarshallingRegistry::GetMarshallingMap(Jigsaw::Util::etype_info::Id<t_class_wrapper>()));
			UNMARSHALLING_RESULT result = reader.BuildNode();

			t_class_wrapper& unmarshalled = *static_cast<t_class_wrapper*>(result.raw_data);
			Assert::AreEqual(1, (int)unmarshalled.t_f_c.t_uids.size());
		}

		TEST_METHOD(TestMarshalSerializableEntityDataAndEnumerations) {
			Jigsaw::Machines::SerializableMachineData machine_data;
			Jigsaw::Entities::SerializableEntityData entity_data;
			entity_data.entity_id = Jigsaw::System::UID::Create();
			entity_data.scope = Jigsaw::Entities::JigsawEntity::SCOPE_STATIC;
			entity_data.aligned_types.push_back(Jigsaw::Util::etype_info::Id<Transform>());

			Transform t;
			t.position = Vector3(0, 0, 5);
			std::string str = MarshallingUtil::Marshal<Transform>(&t);
			entity_data.string_vector.strings.push_back(str);
			machine_data.entity_data.push_back(entity_data);

			std::string marshalled = Jigsaw::Marshalling::MarshallingUtil::Marshal<SerializableMachineData>(&machine_data);

			Ref<SerializableMachineData> data = Jigsaw::Marshalling::MarshallingUtil::Unmarshal<SerializableMachineData>(marshalled);
			Assert::IsTrue(data->entity_data.at(0).scope == Jigsaw::Entities::JigsawEntity::SCOPE_STATIC);

			Ref<Transform> u_transform = Jigsaw::Marshalling::MarshallingUtil::Unmarshal<Transform>(data->entity_data.at(0).string_vector.strings.at(0)); 
			Assert::IsTrue(*u_transform == t);
		}
	};
}
#endif
