#include "DataAssets.h"

#include "Marshalling/JigsawMarshalling.h"
#include <d3dcompiler.h>
#include "Marshalling/JSONNode.h"

using namespace Jigsaw::Assets;
using namespace Jigsaw::File_IO;
using namespace Jigsaw::Marshalling;

// ShaderResource implementations

ASSET_LOAD_RESULT ShaderResource::Load(const FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
	HRESULT hresult = D3DReadFileToBlob(file_data.file_name, &shader);

	if (FAILED(hresult)) {
		Jigsaw::Graphics::DX_Context::PrintDebugMessages();
	}

	bytecode_desc.BytecodeLength = shader->GetBufferSize();
	bytecode_desc.pShaderBytecode = shader->GetBufferPointer();

	ASSET_LOAD_RESULT result;
	result.result = COMPLETE;
	return result;
}

ShaderResource::~ShaderResource() { }

void* Jigsaw::Assets::JsonAsset::GetRawData() {
	return object;
}

const Jigsaw::Util::etype_info& Jigsaw::Assets::JsonAsset::GetTypeInfo() {
	return t_info;
}

ASSET_LOAD_RESULT Jigsaw::Assets::JsonAsset::Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
	ASSET_LOAD_RESULT result;
	std::istringstream ss(GetStringStreamFromFile(file_data));

	JSONNodeReader json_reader(ss, MarshallingRegistry::GetMarshallingMap(t_info));
	UNMARSHALLING_RESULT u_result = json_reader.BuildNode();

	object = u_result.raw_data;
	result.unresolved_references = u_result.unresolved_references;
	result.result = u_result.result;

	return result;
}
