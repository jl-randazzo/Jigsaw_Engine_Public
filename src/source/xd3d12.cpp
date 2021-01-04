#include "xd3d12.h"

D3D12_VERTEX_BUFFER_VIEW GetVBV(const Jigsaw::Graphics::GPUBuffer* gpu_buffer)
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = gpu_buffer->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = gpu_buffer->GetSize();
	vbv.StrideInBytes = gpu_buffer->GetElementSize();
	return vbv;
}

D3D12_INDEX_BUFFER_VIEW GetIBV(const Jigsaw::Graphics::GPUBuffer* gpu_buffer)
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = gpu_buffer->GetResource()->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ibv.SizeInBytes = gpu_buffer->GetSize();
	return ibv;
}
