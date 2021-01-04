//*********************************************************
//
// These extension methods, classes, and utilities were developed
// by Luke Randazzo to streamline some of directX's processes
//
//*********************************************************

#ifndef _X_DX_D12_H_
#define _X_DX_D12_H_

#include "d3d12.h"
#include <wrl.h>
#include "Graphics/GPURuntimeObjects.h"

class XD3D12_CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE {
public:
	XD3D12_CPU_DESCRIPTOR_HANDLE(D3D12_CPU_DESCRIPTOR_HANDLE desc_handle, Microsoft::WRL::ComPtr<ID3D12Device2> dx_device, D3D12_DESCRIPTOR_HEAP_TYPE desc_type)
		: D3D12_CPU_DESCRIPTOR_HANDLE(desc_handle), increment_size(dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)), orig_ptr(desc_handle.ptr) { }

	XD3D12_CPU_DESCRIPTOR_HANDLE& operator ++() {
		ptr = SIZE_T(INT64(ptr) + increment_size);
		return *this;
	}

	XD3D12_CPU_DESCRIPTOR_HANDLE & operator --() {
		ptr = SIZE_T(INT64(ptr) - increment_size);
		return *this;
	}

	XD3D12_CPU_DESCRIPTOR_HANDLE & rewind() {
		ptr = orig_ptr;
		return *this;
	}

	XD3D12_CPU_DESCRIPTOR_HANDLE& get(const UINT i) {
		ptr = SIZE_T(INT64(orig_ptr) + (increment_size * i));
		return *this;
	}

private:

	const INT64 increment_size;
	const INT64 orig_ptr;
};

class XD3D12_COMMAND_QUEUE_DESC : public D3D12_COMMAND_QUEUE_DESC {
private:
	XD3D12_COMMAND_QUEUE_DESC(D3D12_COMMAND_LIST_TYPE type, UINT node_mask) {
		Type = type;
		Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		NodeMask = node_mask;
		Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	}
public:
	static XD3D12_COMMAND_QUEUE_DESC LoadQueue(UINT node_mask) {
		return XD3D12_COMMAND_QUEUE_DESC(D3D12_COMMAND_LIST_TYPE_COPY, node_mask);
	};

	static XD3D12_COMMAND_QUEUE_DESC FrameQueue(UINT node_mask) {
		return XD3D12_COMMAND_QUEUE_DESC(D3D12_COMMAND_LIST_TYPE_DIRECT, node_mask);
	};
};


/**
* Returns a struct describing the vertex buffer view
*/
D3D12_VERTEX_BUFFER_VIEW GetVBV(const Jigsaw::Graphics::GPUBuffer* gpu_buffer); 
/**
* Returns a struct describing the location, format, and size of the index buffer on the GPU
*/
D3D12_INDEX_BUFFER_VIEW GetIBV(const Jigsaw::Graphics::GPUBuffer* gpu_buffer);
#endif
