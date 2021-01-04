#ifdef _RUN_UNIT_TESTS_
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <cmath>
#include "Assets/AssetDatabase.h"
#include "fileapi.h"
#include <string>
#include "Ref.h"
#include "Marshalling/JigsawMarshalling.h"
#include "Assets/DataAssets.h"
#include "TestUtils_b.h"

#define T_SCENE_ID "f1207d0a-4e50-4f6d-8ec6-d4dd4abbdddc"
#define T_MACHINE_ID "87434049-1178-9816-A10C-32BFD586553E"
#define T_CUBE_ID "F244A951-FF44-29BA-8D3A-BD4B4CE08411"

namespace ProjectTests {

	bool withinDelta(float a, float b, float delta = .000001);

	Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter> GetTestDB();
	void WriteTestTableEntries(const Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter>& db);
	void DestroyTestDB(Jigsaw::Ref<Jigsaw::Assets::AssetDatabaseReadWriter>& test_db);

	/// <summary>
	/// Test structs/classes for marshalling/unmarshalling data follow
	/// The registration of their fields is in TestUtils.cpp
	/// </summary>

	struct primitive_type_wrapper {
		bool truth;
		int val;
		long long long_val;
		double d_precision;
		float s_precision;
		std::string str;
	};

	struct t_uid {
		long long data_a;
		long long data_b;

		t_uid(long long data_a = 0, long long data_b = 0) : data_a(data_a), data_b(data_b) {};

		t_uid(const t_uid& other) : data_a(other.data_a), data_b(other.data_b) {}

		virtual bool operator==(const t_uid& other) {
			return data_a == other.data_a && data_b == other.data_b;
		}
	};

	class t_uid_c : public t_uid {
	public:
		long long data_c;
		Jigsaw::Unique<std::string> name;

		t_uid_c(long long data_a = 0, long long data_b = 0, long long data_c = 0) : t_uid(data_a, data_b), data_c(data_c) {};

		bool operator==(const t_uid_c& other) {
			return data_a == other.data_a && data_b == other.data_b && data_c == other.data_c;
		}
	};

	class t_uid_parclass {
	public:
		t_uid main_uid;

		t_uid_parclass() {}

		t_uid_parclass(const t_uid_parclass& other) {
			main_uid = other.main_uid;
		}

		t_uid_parclass& operator=(const t_uid_parclass& other) {
			main_uid = other.main_uid;
			return *this;
		}

		virtual void DoStuff() {

		};
	};

	class t_uid_class : public t_uid_parclass {
	public:
		t_uid_class() : t_uid_parclass(), child_t_uid() {}
		t_uid_class(const t_uid_class& other) {
			CONST_FIELD_ASSIGN(asset_ref) = std::move(other.asset_ref);
			ids[0] = other.ids[0];
			ids[1] = other.ids[1];
			uid_name = other.uid_name;
			root = other.root;
		}

		t_uid_class& operator=(const t_uid_class& other) {
			CONST_FIELD_ASSIGN(asset_ref) = std::move(other.asset_ref);
			ids[0] = other.ids[0];
			ids[1] = other.ids[1];
			uid_name = other.uid_name;
			root = other.root;
			return *this;
		}

		t_uid ids[2];
		std::string uid_name;
		const Jigsaw::Ref<Jigsaw::Assets::ShaderResource> asset_ref;
		Jigsaw::Assets::SerializedRef<t_uid> child_t_uid;
		t_context_root root;
	};

	class t_uid_child_child : public t_uid_class {
	public:
		t_uid_child_child() : t_uid_class() {}

		t_uid_child_child(const t_uid_child_child& other) {
			t_uid* _uid = new t_uid(*other.pointer_object.get());
			pointer_object = Jigsaw::Unique<t_uid>(_uid);
			super_child = other.super_child;
			uid = other.uid;
		}

		Jigsaw::Unique<t_uid> pointer_object;
		std::string super_child;
		Jigsaw::System::UID uid;
	};

	class t_field_class {
	public:
		std::vector<t_uid_class> t_uids;
		int val;
		bool truth_a;
		bool truth_b;
		std::string x;
		t_uid id;
		double d_precision;
		float s_precision;
		std::vector<std::string> str_array;
		std::vector<t_uid*> pointer_array;
	};

	class t_vector_wrapper {
	public:
		std::vector<std::string> str_vector;
		std::vector<int> int_vector;
		std::vector<t_uid> t_uid_vector;
	};

	class t_ptr_class {
	public:
		std::vector<t_uid*> ptr_array;
		Jigsaw::Unique<t_uid> t_u_c;
	};

	class t_ref_bound {
	public:
		t_uid t_uids[2];
		Jigsaw::Ref<Jigsaw::Assets::ShaderResource> asset_ref;
	};

	class t_class_wrapper {
	public:
		t_field_class t_f_c;
		Jigsaw::Assets::SerializedRef<t_uid> s_ref;
	};
}

#endif // !_TEST_UTILS_H_
#endif
