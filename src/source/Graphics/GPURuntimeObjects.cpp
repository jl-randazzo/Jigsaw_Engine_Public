#include "GPURuntimeObjects.h"

#ifdef _J_WIN_10_
Microsoft::WRL::ComPtr<ID3D12RootSignature>& Jigsaw::Graphics::RootSignatureObject::GetRootSignature() {
	return root_signature;
}
#endif

size_t Jigsaw::Graphics::GPUBuffer::GetSize() const
{
	return size;
}

size_t Jigsaw::Graphics::GPUBuffer::GetElementSize() const
{
	return element_size;
}

Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE Jigsaw::Graphics::PipelineObject::GetTopologyType() const {
	return topology_type;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>& Jigsaw::Graphics::PipelineObject::GetPipelineState() {
	return this->ps_object;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature>& Jigsaw::Graphics::PipelineObject::GetRootSignature() {
	return rs_object->GetRootSignature();
}
