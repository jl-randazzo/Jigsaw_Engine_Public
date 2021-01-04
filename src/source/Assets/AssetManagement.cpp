#include "AssetManagement.h"
#include <filesystem>
#include <cstdlib>
#include <wrl.h>
#include "Debug/j_log.h"

#ifdef _RUN_UNIT_TESTS_
#include "TestResource.h"
#endif

using namespace Jigsaw::Assets;
using namespace Jigsaw::System;

D3D12_ROOT_SIGNATURE_FLAGS root_flags = 
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | 
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | 
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

/**
* Basic internal utility for returning the inherited type based on the descriptor type
*/
Jigsaw::Ref<DataAsset> GetNewResourceByType(const AssetDescriptor& descriptor) {
	switch (descriptor.type) {
#ifdef _RUN_UNIT_TESTS_
	case ASSET_TEST:
		return std::dynamic_pointer_cast<DataAsset>(Jigsaw::MakeRef<TestResource>(descriptor));
#endif
	case ASSET_VERTEX_SHADER:
	case ASSET_PIXEL_SHADER:
	case ASSET_GEOMETRY_SHADER:
		return std::dynamic_pointer_cast<DataAsset>(Jigsaw::MakeRef<ShaderResource>(descriptor));
	case ASSET_JSON:
		return std::dynamic_pointer_cast<DataAsset>(Jigsaw::MakeRef<JsonAsset>(descriptor));
	case ASSET_CUBE:
		return std::dynamic_pointer_cast<DataAsset>(Jigsaw::MakeRef<Cube>(descriptor, 1));
	}
}

Jigsaw::Ref<DataAsset> Jigsaw::Assets::FetchAsset(const char* file_name, ASSET_TYPE type, THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
	Ref<DataAsset> resource;
	AssetDescriptor descriptor;
	descriptor.type = type;
	descriptor.id = Jigsaw::System::UID::Create();
	descriptor.file_path = file_name;

	// get initial resource
	resource = GetNewResourceByType(descriptor);

	// load the file needed to initialize the resource
	Jigsaw::File_IO::FILE_DATA resource_file_data = Jigsaw::File_IO::GetResourceFile(descriptor.file_path);

	resource->Load(resource_file_data, sys_resources);
	fclose(resource_file_data.file);
	delete[] resource_file_data.file_name;

	AssetManagerOwner::GetManager().WriteClaimedAsset(resource);
	return resource;
}

/// <summary>
/// Internal helper function
/// </summary>
/// <param name="root_object"></param>
/// <param name="unresolved_references"></param>
void ResolveReferences(std::vector<UNRESOLVED_REFERENCE>& unresolved_references, THREAD_SAFE_SYSTEM_RESOURCES& sys_resources) {
	Jigsaw::Unique<UID[]> reference_ids(new UID[unresolved_references.size()]);
	for (int i = 0; i < unresolved_references.size(); i++) {
		reference_ids[i] = unresolved_references[i].ref_id;
		J_LOG_INFO(AssetManager, "Fetching unresolved asset reference id {0}", Jigsaw::System::UIDToString(reference_ids[i]));
	}

	Jigsaw::Unique<Jigsaw::Ref<DataAsset>[]> resolved_references = FetchAssets(reference_ids, unresolved_references.size(), sys_resources);

	for (int i = 0; i < unresolved_references.size(); i++) {
		unresolved_references[i].injector(&resolved_references[i]);
	}
}

Jigsaw::Unique<Jigsaw::Ref<DataAsset>[]> Jigsaw::Assets::FetchAssets(const Jigsaw::Unique<Jigsaw::System::UID[]>& uids, const int count, THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
	Ref<DataAsset>* resources = new Ref<DataAsset>[count];
	const UID* uids_ = uids.get();

	AssetManager& manager = AssetManagerOwner::GetManager();

	for (int i = 0; i < count; i++) {
		// check if the resource is already available in the map
		if (!manager.ClaimAsset(uids_[i], &resources[i])) {
			AssetDescriptor descriptor = sys_resources.db->FetchDescriptor(uids_[i]);
			Jigsaw::File_IO::FILE_DATA resource_file_data = Jigsaw::File_IO::GetResourceFile(descriptor.file_path);
			resources[i] = GetNewResourceByType(descriptor);

			ASSET_LOAD_RESULT result = resources[i]->Load(resource_file_data, sys_resources);
			fclose(resource_file_data.file);

			manager.WriteClaimedAsset(resources[i]);

			if (result.result == UNRESOLVED_REFERENCES) {
				ResolveReferences(result.unresolved_references, sys_resources);
			}
		} 
	}

	return Unique<Ref<DataAsset>[]>(resources);
}

// base implementation for the compiler. We need to signal that it is purely abstract
BaseAsset::~BaseAsset() { }

void Jigsaw::Assets::AssetManager::FindAsset(const Jigsaw::System::UID& uid, ASSET_DATA_BLOCK& ref_block) {
	auto iter = resource_map.find(uid);
	if (iter != resource_map.end()) {
		ref_block = iter->second;
	} else {
		J_LOG_TRACE(AssetManager, "Asset non-existent in map history {0}", Jigsaw::System::UIDToString(uid));
		ref_block.state = ASSET_STATE_INACTIVE;
	}
}

void Jigsaw::Assets::AssetManager::MarkAssetForLoading(const Jigsaw::System::UID& uid) {
	J_LOG_TRACE(AssetManager, "Marking asset to be loaded: {0}", Jigsaw::System::UIDToString(uid));
	auto iter = resource_map.find(uid);
	ASSET_DATA_BLOCK block;
	block.state = ASSET_STATE_LOADING;

	if (iter != resource_map.end()) {
		resource_map[uid] = block;
	} else {
		resource_map.insert(std::make_pair(uid, block));
	}
}

bool Jigsaw::Assets::AssetManager::ClaimAsset(const Jigsaw::System::UID& uid, Jigsaw::Ref<Jigsaw::Assets::DataAsset>* out_ref) {
	ASSET_DATA_BLOCK read_block;
	goto SPIN;

	J_LOG_TRACE(AssetManager, "Attempting to claim asset {0}", Jigsaw::System::UIDToString(uid));

	while (read_block.state == ASSET_STATE_LOADING) {
		J_LOG_TRACE(AssetManager, "Spin cycle engaged for loading asset {0}", Jigsaw::System::UIDToString(uid));
		Sleep(1);
	SPIN:
		std::scoped_lock<std::mutex> l(lock);
		FindAsset(uid, read_block);

		if (read_block.state == ASSET_STATE_ACTIVE) {
			if (*out_ref = read_block.ref.lock()) {
				J_LOG_TRACE(AssetManager, "Active reference discovered for asset {0}", Jigsaw::System::UIDToString(uid));
				return true;
			} else {
				J_LOG_TRACE(AssetManager, "Inactive reference discovered for asset {0}", Jigsaw::System::UIDToString(uid));
				read_block.state = ASSET_STATE_INACTIVE;
			}
		}

		if (read_block.state == ASSET_STATE_INACTIVE) {
			MarkAssetForLoading(uid);
		}
	}
	return false;
}

bool Jigsaw::Assets::AssetManager::WriteClaimedAsset(Jigsaw::Ref<Jigsaw::Assets::DataAsset>& claimed_asset) {
	std::scoped_lock<std::mutex> l(lock);

	ASSET_DATA_BLOCK new_block;
	new_block.ref = claimed_asset;
	new_block.state = ASSET_STATE_ACTIVE;
	resource_map[claimed_asset->GetDescriptor().id] = new_block;

	return true;
}

AssetManager& Jigsaw::Assets::AssetManagerOwner::GetManager() {
	static AssetManagerOwner instance;
	return instance;
}

