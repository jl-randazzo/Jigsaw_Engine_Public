#ifdef _RUN_UNIT_TESTS_
#include "CppUnitTest.h"
#include "sqlite3.h"
#include "Assets/AssetManagement.h"
#include "Assets/AssetDatabase.h"
#include "TestUtils.h"
#include "Ref.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Assets;
using namespace Jigsaw::System;
using namespace Jigsaw;

#define WRITE_COUNT 5
#define READ_COUNT 10
#define SCENE_DESCRIPTOR_COUNT 3

namespace ProjectTests {
	TEST_CLASS(TestAssetDatabase)
	{

		static UID scene_id;
		static Ref<Jigsaw::Assets::AssetDatabaseReadWriter> db;
		static AssetDescriptor* test_desc;
		static Unique<AssetDescriptor[]> machine_descriptors;
		static Unique<UID[]> machine_ids;
		static Jigsaw::Unique<UID[]> read_machine_ids;

		TEST_CLASS_INITIALIZE(Setup) {
			db = GetTestDB();

			test_desc = new AssetDescriptor();
			test_desc->id = Jigsaw::System::UID::Create();
			test_desc->file_path = "./unit_test.db";
			test_desc->type = ASSET_MODEL;
			db->WriteDescriptor(*test_desc);

			machine_descriptors = Unique<AssetDescriptor[]>(new AssetDescriptor[SCENE_DESCRIPTOR_COUNT]);
			for (int i = 0; i < SCENE_DESCRIPTOR_COUNT; i++) {
				AssetDescriptor& descriptor = machine_descriptors[i];
				descriptor.id = UID::Create();
				descriptor.type = ASSET_JSON;
				descriptor.file_path = "./";
				descriptor.fully_qualified_type_name = "Jigsaw::Machines::JigsawMachine";
				db->WriteDescriptor(descriptor);
			}
			machine_ids = UniqueMap<AssetDescriptor, UID>(machine_descriptors, SCENE_DESCRIPTOR_COUNT, [](AssetDescriptor& ad) -> UID { return ad.id; });
			size_t out_size = 0;
			scene_id = UID::Create();
			db->WriteSceneMachineAssociations(scene_id, machine_ids, SCENE_DESCRIPTOR_COUNT);
			read_machine_ids = db->FetchSceneToMachineAssociations(scene_id, &out_size);

		}

		TEST_CLASS_CLEANUP(Cleanup) {
			DestroyTestDB(db);
			db.reset();
			delete test_desc;
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestAssetDatabaseReader)
			TEST_PRIORITY(2)
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(TestAssetDatabaseReader) {
			AssetDescriptor t_desc = *test_desc;
			t_desc.id = UID::Create();
			db->WriteDescriptor(t_desc);

			AssetDatabaseReader reader("./unit_test.db");
			AssetDescriptor descriptor_b = reader.FetchDescriptor(t_desc.id);

			Assert::IsTrue(t_desc.id == descriptor_b.id);
			Assert::AreEqual(*t_desc.file_path, *descriptor_b.file_path);
			Assert::IsTrue(t_desc.type == descriptor_b.type);
			Assert::IsTrue(ASSET_MODEL == descriptor_b.type);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(TestAssetDatabaseFetchAssociatedSceneDescriptors)
			TEST_PRIORITY(5)
			END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestAssetDatabaseFetchAssociatedSceneDescriptors) {
			size_t out_size = 0;
			Unique<UID[]> read_machine_ids = db->FetchSceneToMachineAssociations(scene_id, &out_size);
			Assert::AreEqual(SCENE_DESCRIPTOR_COUNT, (int)out_size);
		}
		
		BEGIN_TEST_METHOD_ATTRIBUTE(TestAssetDatabaseFetchResourceDescriptorsFromSceneDescriptors)
			TEST_PRIORITY(5)
			END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestAssetDatabaseFetchResourceDescriptorsFromSceneDescriptors) {
			size_t out_size = 0;
			Unique<UID[]> read_machine_ids = db->FetchSceneToMachineAssociations(scene_id, &out_size);

			for (int i = 0; i < SCENE_DESCRIPTOR_COUNT; i++) {
				AssetDescriptor read_desc = db->FetchDescriptor(read_machine_ids.get()[i]);
				Assert::IsTrue(read_desc == machine_descriptors[i]);
			}
		}
	};

	// static members
	UID TestAssetDatabase::scene_id;
	Unique<AssetDescriptor[]> TestAssetDatabase::machine_descriptors;
	Jigsaw::Unique<UID[]> TestAssetDatabase::read_machine_ids;
	Unique<UID[]> TestAssetDatabase::machine_ids;
	Ref<Jigsaw::Assets::AssetDatabaseReadWriter> TestAssetDatabase::db;
	AssetDescriptor* TestAssetDatabase::test_desc;
}
#endif
