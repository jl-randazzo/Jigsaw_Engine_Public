
#ifdef _RUN_UNIT_TESTS_
#include "TestUtils.h"
#include "Assets/BaseAsset.h"

using namespace ProjectTests;

bool ProjectTests::withinDelta(float a, float b, float delta) {
	return std::abs(a - b) < delta;
}

Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter> ProjectTests::GetTestDB() {
	BOOL a = DeleteFileA("./unit_test.db");
	return Jigsaw::MakeRef<Jigsaw::Assets::AssetDatabaseReadWriter>("./unit_test.db");
}

void ProjectTests::WriteTestTableEntries(const Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter>& db) {
	Jigsaw::Assets::AssetDescriptor machine_desc;
	machine_desc.fully_qualified_type_name = "Jigsaw::Machines::JigsawMachine";
	machine_desc.file_path = "./Assets/test_jigsaw_machine.json";
	machine_desc.type = Jigsaw::Assets::ASSET_JSON;
	machine_desc.id = Jigsaw::System::UIDFromString(T_MACHINE_ID);
	db->WriteDescriptor(machine_desc);

	Jigsaw::Assets::AssetDescriptor cube_desc;
	cube_desc.type = Jigsaw::Assets::ASSET_CUBE;
	cube_desc.id = Jigsaw::System::UIDFromString(T_CUBE_ID);
	cube_desc.file_path = "./Assets/cube.txt";
	cube_desc.fully_qualified_type_name = "";
	db->WriteDescriptor(cube_desc);

	Jigsaw::Unique<Jigsaw::System::UID[]> ids = Jigsaw::Unique<Jigsaw::System::UID[]>(new Jigsaw::System::UID(machine_desc.id));
	Jigsaw::System::UID scene_id = Jigsaw::System::UIDFromString(T_SCENE_ID);

	db->WriteSceneMachineAssociations(scene_id, ids, 1);
}

void ProjectTests::DestroyTestDB(Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter>& test_db) {
	test_db.reset();
	BOOL a = DeleteFileA("./unit_test.db");

}

START_REGISTER_SERIALIZABLE_CLASS(t_uid)
REGISTER_SERIALIZABLE_FIELD(t_uid, long long, data_a)
REGISTER_SERIALIZABLE_FIELD(t_uid, long long, data_b)
END_REGISTER_SERIALIZABLE_CLASS(t_uid)

START_REGISTER_SERIALIZABLE_CLASS(t_uid_c)
REGISTER_SERIALIZABLE_CHILD_OF(t_uid_c, t_uid)
REGISTER_SERIALIZABLE_FIELD(t_uid_c, long long, data_c)
REGISTER_SERIALIZABLE_POINTER(t_uid_c, std::string, name)
END_REGISTER_SERIALIZABLE_CLASS(t_uid_c)

START_REGISTER_SERIALIZABLE_CLASS(t_uid_parclass)
REGISTER_SERIALIZABLE_FIELD(t_uid_parclass, t_uid, main_uid)
END_REGISTER_SERIALIZABLE_CLASS(t_uid_parclass)

START_REGISTER_SERIALIZABLE_CLASS(t_uid_child_child)
REGISTER_SERIALIZABLE_CHILD_OF(t_uid_child_child, t_uid_class)
REGISTER_SERIALIZABLE_FIELD(t_uid_child_child, std::string, super_child)
REGISTER_SERIALIZABLE_FIELD(t_uid_child_child, Jigsaw::System::UID, uid)
REGISTER_SERIALIZABLE_POINTER(t_uid_child_child, t_uid, pointer_object)
END_REGISTER_SERIALIZABLE_CLASS(t_uid_child_child)

START_REGISTER_SERIALIZABLE_CLASS(t_uid_class)
REGISTER_SERIALIZABLE_CHILD_OF(t_uid_class, t_uid_parclass)
REGISTER_SERIALIZABLE_ASSET_REFERENCE(t_uid_class, Jigsaw::Assets::ShaderResource, asset_ref)
REGISTER_SERIALIZABLE_REFERENCE(t_uid_class, t_uid, child_t_uid)
REGISTER_SERIALIZABLE_BOUNDED_ARRAY(t_uid_class, t_uid, 2, ids)
REGISTER_SERIALIZABLE_FIELD(t_uid_class, std::string, uid_name)
REGISTER_SERIALIZABLE_FIELD(t_uid_class, t_context_root, root)
END_REGISTER_SERIALIZABLE_CLASS(t_uid_class)

START_REGISTER_SERIALIZABLE_CLASS(t_field_class)
REGISTER_SERIALIZABLE_VECTOR(t_field_class, std::string, str_array)
REGISTER_SERIALIZABLE_FIELD(t_field_class, int, val)
REGISTER_SERIALIZABLE_FIELD(t_field_class, bool, truth_a)
REGISTER_SERIALIZABLE_FIELD(t_field_class, bool, truth_b)
REGISTER_SERIALIZABLE_FIELD(t_field_class, std::string, x)
REGISTER_SERIALIZABLE_FIELD(t_field_class, t_uid, id)
REGISTER_SERIALIZABLE_FIELD(t_field_class, double, d_precision)
REGISTER_SERIALIZABLE_FIELD(t_field_class, float, s_precision)
REGISTER_SERIALIZABLE_VECTOR(t_field_class, t_uid_class, t_uids)
REGISTER_SERIALIZABLE_POINTER_VECTOR(t_field_class, t_uid, pointer_array)
END_REGISTER_SERIALIZABLE_CLASS(t_field_class)

START_REGISTER_CONTEXT_ROOTED_SERIALIZABLE_CLASS(t_context_root, t_uid_class)
END_REGISTER_SERIALIZABLE_CLASS(t_context_root)

START_REGISTER_SERIALIZABLE_CLASS(primitive_type_wrapper)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, int, val)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, bool, truth)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, long long, long_val)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, double, d_precision)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, float, s_precision)
REGISTER_SERIALIZABLE_FIELD(primitive_type_wrapper, std::string, str)
END_REGISTER_SERIALIZABLE_CLASS(primitive_type_wrapper)

START_REGISTER_SERIALIZABLE_CLASS(t_vector_wrapper)
REGISTER_SERIALIZABLE_VECTOR(t_vector_wrapper, std::string, str_vector)
REGISTER_SERIALIZABLE_VECTOR(t_vector_wrapper, int, int_vector)
REGISTER_SERIALIZABLE_VECTOR(t_vector_wrapper, t_uid, t_uid_vector)
END_REGISTER_SERIALIZABLE_CLASS(t_vector_wrapper)

START_REGISTER_SERIALIZABLE_CLASS(t_ptr_class)
REGISTER_SERIALIZABLE_POINTER_VECTOR(t_ptr_class, t_uid, ptr_array)
REGISTER_SERIALIZABLE_POINTER(t_ptr_class, t_uid, t_u_c)
END_REGISTER_SERIALIZABLE_CLASS(t_ptr_class)

START_REGISTER_SERIALIZABLE_CLASS(t_ref_bound)
REGISTER_SERIALIZABLE_BOUNDED_ARRAY(t_ref_bound, t_uid, 2, t_uids)
REGISTER_SERIALIZABLE_ASSET_REFERENCE(t_ref_bound, Jigsaw::Assets::ShaderResource, asset_ref)
END_REGISTER_SERIALIZABLE_CLASS(t_ref_bound)

START_REGISTER_SERIALIZABLE_CLASS(t_class_wrapper)
REGISTER_SERIALIZABLE_FIELD(t_class_wrapper, t_field_class, t_f_c)
REGISTER_SERIALIZABLE_REFERENCE(t_class_wrapper, t_uid, s_ref)
END_REGISTER_SERIALIZABLE_CLASS(t_class_wrapper)
#endif
