#ifdef _RUN_UNIT_TESTS_

#include "CppUnitTest.h"
#include "Assets/AssetManagement.h"
#include "Assets/AssetDatabase.h"
#include "TestUtils.h"
#include <thread>
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Jigsaw::Assets;
using namespace Jigsaw::System;
using namespace Jigsaw;

#define MULTI_THREAD_COUNT 3
#define MULTI_THREAD_RUNS 3

namespace ProjectTests {
	TEST_CLASS(TestAssetManagement)
	{
		static Ref<AssetDatabaseReadWriter> db;
		static UID* texture_uid;
		static UID* text_uid;
		static UID* json_uid;

		TEST_CLASS_INITIALIZE(SetupClass) {
			db = Ref<AssetDatabaseReadWriter>(GetTestDB());

			AssetDescriptor descriptor;
			descriptor.id = Jigsaw::System::UID::Create();
			descriptor.file_path = "./Assets/test.png";
			descriptor.type = ASSET_TEST;
			db->WriteDescriptor(descriptor);
			texture_uid = new Jigsaw::System::UID(descriptor.id);

			descriptor.id = Jigsaw::System::UID::Create();
			descriptor.file_path = "./Assets/text.txt";
			descriptor.type = ASSET_TEST;
			db->WriteDescriptor(descriptor);
			text_uid = new Jigsaw::System::UID(descriptor.id);

			// the following descriptors reference real test assets
			std::string ref_uid("70cf97fd-c1d1-4dd9-b7e1-3784b2c4dd65");
			Jigsaw::System::UIDFromString(&descriptor.id, &ref_uid);
			descriptor.file_path = "./Assets/TestAssetManagement/child_uid_reference.json";
			descriptor.fully_qualified_type_name = "ProjectTests::t_uid";
			descriptor.type = ASSET_JSON;
			db->WriteDescriptor(descriptor);

			descriptor.id = Jigsaw::System::UID::Create();
			descriptor.file_path = "./Assets/TestAssetManagement/t_uid_class_ref.json";
			descriptor.fully_qualified_type_name = "ProjectTests::t_uid_class";
			descriptor.type = ASSET_JSON;
			json_uid = new Jigsaw::System::UID(descriptor.id);
			db->WriteDescriptor(descriptor);
		}

		TEST_CLASS_CLEANUP(CleanupClass) {
			DestroyTestDB(db);
			db.reset();
			delete texture_uid;
			delete text_uid;
		}

		THREAD_SAFE_SYSTEM_RESOURCES sys_resources;
		THREAD_SAFE_SYSTEM_RESOURCES* sys_resources_pool;

		TEST_METHOD_INITIALIZE(Setup) {
			sys_resources.db = std::dynamic_pointer_cast<AssetDatabaseReader>(db);
			sys_resources_pool = new THREAD_SAFE_SYSTEM_RESOURCES[MULTI_THREAD_COUNT];
			for (int i = 0; i < MULTI_THREAD_COUNT; i++) {
				sys_resources_pool[i].db = MakeRef<AssetDatabaseReader>("./unit_test.db");
			}
		}

		TEST_METHOD_CLEANUP(Cleanup) {
			delete[] sys_resources_pool;
		}

		TEST_METHOD(TestFetchResource) {
			Unique<UID[]> uid_array = Unique<UID[]>(new UID[1]{ *text_uid });
			Unique<Ref<DataAsset>[]> assets = FetchAssets(uid_array, 1, sys_resources);
			Ref<DataAsset> asset = (assets.get())[0];
			Assert::IsTrue(asset->GetDescriptor().id == *text_uid);
		}

		void TestScopedMultiThreadRun() {
			std::cout << "Runing test" << std::endl;
			Unique<UID[]> uid_array = Unique<UID[]>(new UID[1]{ *text_uid });
			Unique<UID[]> uid_array_alt = Unique<UID[]>(new UID[1]{ *texture_uid });
			std::promise<Unique<Ref<DataAsset>[]>> promises[MULTI_THREAD_COUNT];
			std::future<Unique<Ref<DataAsset>[]>> futures[MULTI_THREAD_COUNT];
			std::thread* threads[MULTI_THREAD_COUNT];

			for (int i = 0; i < MULTI_THREAD_COUNT; i++) {
				const Unique<UID[]>& uid = i % 2 == 0 ? uid_array : uid_array_alt;
				THREAD_SAFE_SYSTEM_RESOURCES sys_r = sys_resources_pool[i];

				futures[i] = promises[i].get_future();
				threads[i] = new std::thread([=,&uid](std::promise<Unique<Ref<DataAsset>[]>>&& promise) -> void {
					Unique<Ref<DataAsset>[]> unique = FetchAssets(uid, 1, sys_r);
					promise.set_value(std::move(unique));
				}, std::move(promises[i]));
			}

			for (int i = 0; i < MULTI_THREAD_COUNT; i++) {
				threads[i]->join();
			}

			Unique<Ref<DataAsset>[]> unique = futures[0].get();
			Ref<DataAsset> asset = unique.get()[0];
			Unique<Ref<DataAsset>[]> unique_alt = futures[1].get();
			Ref<DataAsset> asset_alt = unique_alt.get()[0];

			for (int i = 2; i < MULTI_THREAD_COUNT; i++) {
				Ref<DataAsset> comp_asset = i % 2 == 0 ? asset : asset_alt;
				Ref<DataAsset> asset_o = futures[i].get().get()[0];
				// comparing by reference to ensure we got the same exact object back
				Assert::IsTrue(comp_asset.get() == asset_o.get());
			}
			
			Assert::IsTrue(asset->GetDescriptor().id == *text_uid);
			Assert::IsTrue(asset_alt->GetDescriptor().id == *texture_uid);

			for (int i = 0; i < MULTI_THREAD_COUNT; i++) {
				delete threads[i];
			}
		}

		TEST_METHOD(TestMultiThreadResolvesToSameResource) {
			for (int i = 0; i < MULTI_THREAD_RUNS; i++) {
				TestScopedMultiThreadRun();
			}
		}

		TEST_METHOD(TestReturnSerializableAssetReference) {
			Unique<UID[]> uid_array = Unique<UID[]>(new UID[1]{ *json_uid });
			Unique<Ref<DataAsset>[]> fetched_assets = FetchAssets(uid_array, 1, sys_resources_pool[0]);
			Ref<JsonAsset> json_asset = std::dynamic_pointer_cast<JsonAsset>(fetched_assets.get()[0]);
			t_uid_class* object = static_cast<t_uid_class*>(json_asset.get()->GetRawData());

			// pulling the serialized reference object
			Jigsaw::Assets::SerializedRef<t_uid> child(object->child_t_uid);

			t_uid* child_obj = child.operator->();
			Assert::IsNotNull(child_obj);

			// precise values can be seen in child_uid_reference.json file
			Assert::IsTrue(8239749283749 == object->child_t_uid->data_a);
			Assert::IsTrue(-123871290012 == object->child_t_uid->data_b);

			Unique<UID[]> uid_array_b = Unique<UID[]>(new UID[1]{ *json_uid });
			Unique<Ref<DataAsset>[]> fetched_assets_b = FetchAssets(uid_array_b, 1, sys_resources_pool[0]);
			Ref<JsonAsset> json_asset_b = std::dynamic_pointer_cast<JsonAsset>(fetched_assets.get()[0]);

			Assert::IsTrue(json_asset.get() == json_asset_b.get());
		}

	};

	Ref<AssetDatabaseReadWriter> TestAssetManagement::db;
	UID* TestAssetManagement::texture_uid;
	UID* TestAssetManagement::text_uid;
	UID* TestAssetManagement::json_uid;
}
#endif
