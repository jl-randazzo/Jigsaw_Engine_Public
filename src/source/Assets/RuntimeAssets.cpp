#include "RuntimeAssets.h"
#include "Graphics/DirectX/DX_Context.h"

/*
using namespace Jigsaw::Assets;

typedef std::pair<const ID3D11VertexShader*, Microsoft::WRL::ComPtr<ID3D11InputLayout>> v_pair;

void InLayoutResource::Initialize(const D3D12_INPUT_ELEMENT_DESC* data, const size_t size) {
	this->size = size;
	this->data = new D3D12_INPUT_ELEMENT_DESC[size];
	memcpy(this->data, data, size * sizeof(D3D11_INPUT_ELEMENT_DESC));
}

void InLayoutResource::Update(const D3D12_INPUT_ELEMENT_DESC* data, const size_t update_size) {
	// InLayoutResources do not update 
}

ID3D11InputLayout* InLayoutResource::GetLayout(const VertexShaderResource& shader_resource) {
	const ID3D11VertexShader* shader_ptr = 0;

	auto found = vshader_layout_map.find(shader_ptr);
	auto end = vshader_layout_map.end();
	if (found != end) {
		return found->second.Get();
	}
	else {
		// if an inputlayout object is unavailable for this vertex shader, then we need to initialize one that can be used later
		size_t sb_size = 1;
		const BYTE* shader_bytes = 0;// = shader_resource.getShaderBytes(&sb_size);
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
		//HRESULT inputResult = DX_Context::GetDevice()->CreateInputLayout(data, size, shader_bytes, sb_size, &input_layout);
		//if (FAILED(inputResult)) {
			//DX_Context::PrintDebugMessages();
		//}

		vshader_layout_map.insert(v_pair(shader_ptr, input_layout));
		return input_layout.Get();
	}
}

//RSResource implementations
void RSResource::Initialize(const D3D11_RASTERIZER_DESC* data, const size_t size) {
	//HRESULT res = DX_Context::GetDevice()->CreateRasterizerState(data, &rasterizer_state);
	//if (FAILED(res)) {
		//DX_Context::PrintDebugMessages();
	//}
}

void RSResource::Update(const D3D11_RASTERIZER_DESC* data, const size_t size) {
	// no impl
}

D3D12_STREAM_OUTPUT_BUFFER_VIEW SOBufferResource::GetSOBV() const {
	D3D12_STREAM_OUTPUT_BUFFER_VIEW sobv;
	sobv.BufferLocation = base_resource->GetGPUVirtualAddress();
	sobv.SizeInBytes = size;
	sobv.BufferFilledSizeLocation = sobv.BufferLocation + size; // last byte is used to write how much has been written
	return sobv;
}

// SOBufferResource implementations
void SOBufferResource::Initialize(const SO_DESC* data, const size_t size) {
	this->size = (*data).size;
	// add on 8 bytes for writing buffer filled data to the end
	CD3DX12_RESOURCE_DESC so_desc = CD3DX12_RESOURCE_DESC::Buffer((*data).size + sizeof(UINT64));
	HRESULT res = DX_Context::GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&so_desc, D3D12_RESOURCE_STATE_STREAM_OUT, nullptr, IID_PPV_ARGS(&base_resource));
	resource_state = D3D12_RESOURCE_STATE_STREAM_OUT;

	if (FAILED(res)) {
		DX_Context::PrintDebugMessages();
	}
}

void SOBufferResource::Update(const SO_DESC* data, const size_t size) {
}

void CopyBufferResource::Initialize(const COPY_DESC* data, const size_t size) {
	this->size = (*data).size;
	CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer((*data).size);
	HRESULT res = DX_Context::GetDevice()->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), D3D12_HEAP_FLAG_NONE,
			&resource_desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&base_resource));
	resource_state = D3D12_RESOURCE_STATE_COPY_DEST;

	if (FAILED(res)) {
		DX_Context::PrintDebugMessages();
	}
}

void CopyBufferResource::Update(const COPY_DESC* data, const size_t size) {

}

void RootSignatureResource::Initialize(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC* data, const size_t size) {

	Microsoft::WRL::ComPtr<ID3DBlob> root_sig_blob;
	Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
	D3DX12SerializeVersionedRootSignature(data, D3D_ROOT_SIGNATURE_VERSION_1_1, &root_sig_blob, &error_blob);

	HRESULT rs_creation = DX_Context::GetDevice()->CreateRootSignature(FRAME_NODE, root_sig_blob->GetBufferPointer(), root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
	PRINT_HRESULT_ERRORS_THROW("Failed to create Root Signature: ", rs_creation);

}

ID3D12RootSignature* RootSignatureResource::GetRootSignature() const {
	return root_signature.Get();
}

void RootSignatureResource::Update(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC* data, const size_t size){
}

// PipelineStateResource implementations
void PipelineStateResource::Initialize(const PSO_DATA* data, const size_t size) {
	this->rs_resource = data[0].rs_resource;

	D3D12_PIPELINE_STATE_STREAM_DESC stream_desc{
		data[0].size - 1, data[0].data
	};

	auto device = DX_Context::GetDevice();
	HRESULT pso_result = device->CreatePipelineState(&stream_desc, __uuidof(ID3D12PipelineState), &ps_object);
	DX_Context::PrintDebugMessages();
	PRINT_HRESULT_ERRORS_THROW("Failed to create PSO: ", pso_result);
}

void PipelineStateResource::Update(const PSO_DATA* data, const size_t size) { }

ID3D12PipelineState* PipelineStateResource::GetPipelineState() const {
	return ps_object.Get();
}

ID3D12RootSignature* PipelineStateResource::GetRootSignature() const {
	return rs_resource->GetRootSignature();
}
*/
