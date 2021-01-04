#include "Graphics/DirectX/DX_CommandList.h"
#include "xd3d12.h"
#include "Graphics/DirectX/DirectXDataLayouts.h"

using namespace Microsoft::WRL;

namespace Jigsaw {
	namespace Graphics {
		void DX_CommandList::SetViews(D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle, D3D12_VIEWPORT& viewport) {
			dx_cmd_list->OMSetRenderTargets(1, rtv_handle, FALSE, dsv_handle);
			dx_cmd_list->RSSetViewports(1, &viewport);
			CD3DX12_RECT scissor(0, 0, LONG_MAX, LONG_MAX);
			dx_cmd_list->RSSetScissorRects(1, &scissor);
		}

		// DX_CommandList implementations
		void DX_CommandList::LoadBuffer(BYTE* t_arr, size_t t_size, size_t t_count, Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>* buffer_dest) {
			command_list_dirty = true;
			*buffer_dest = Jigsaw::MakeRef<Jigsaw::Graphics::GPUBuffer>(t_count * t_size, t_size);

			CD3DX12_RESOURCE_DESC vb_desc = CD3DX12_RESOURCE_DESC::Buffer(t_size * t_count);

			// creating the vertex buffer and copy buffer
			ComPtr<ID3D12Device2> device;
			dx_cmd_list->GetDevice(IID_PPV_ARGS(&device));
			device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&vb_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&(*buffer_dest)->GetResourceReference()));
			device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&vb_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(*buffer_dest)->GetCopyResource()));

			D3D12_SUBRESOURCE_DATA vertex_data;
			vertex_data.pData = t_arr;
			vertex_data.RowPitch = t_count;
			vertex_data.SlicePitch = t_count;

			UINT64 updateRes = UpdateSubresources(dx_cmd_list.Get(), (*buffer_dest)->GetResource().Get(), (*buffer_dest)->GetCopyResource().Get(), 0, 0, 1, &vertex_data);

			loading_buffers.push_back(buffer_dest->get());
		}

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DX_CommandList::GetDXCMDList() {
			return dx_cmd_list;
		}

		ID3D12GraphicsCommandList* DX_CommandList::operator->() {
			return dx_cmd_list.Get();
		}

		D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE type) {
			switch (type) {
			case JGSW_TOPOLOGY_TYPE_TRIANGLE:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case JGSW_TOPOLOGY_TYPE_LINE:
			default:
				return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			}
		}

		void DX_CommandList::DrawIndexed(Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& pl_object, RENDER_DATA& render_data) {
			command_list_dirty = true;
			dx_cmd_list->SetPipelineState(pl_object->GetPipelineState().Get());
			dx_cmd_list->SetGraphicsRootSignature(pl_object->GetRootSignature().Get());
			dx_cmd_list->IASetPrimitiveTopology(ConvertPrimitiveTopology(pl_object->GetTopologyType()));

			D3D12_VERTEX_BUFFER_VIEW vbv = GetVBV(render_data.v_buffer.get());
			dx_cmd_list->IASetVertexBuffers(0, 1, &vbv);
			D3D12_INDEX_BUFFER_VIEW ibv = GetIBV(render_data.i_buffer.get());
			dx_cmd_list->IASetIndexBuffer(&ibv);

			MVPMatrixConstant mvpUpdateValue;
			Mat4x4 pvmMat = render_data.pvMat * render_data.t.getTRSMatrix();

			mvpUpdateValue.modelRotMatrix = render_data.t.getRotationMatrix();
			mvpUpdateValue.mvpMatrix = pvmMat;

			dx_cmd_list->SetGraphicsRoot32BitConstants(0, sizeof(MVPMatrixConstant) / 4, &mvpUpdateValue, 0);

			dx_cmd_list->DrawIndexedInstanced(ibv.SizeInBytes / 4, 1, 0, 0, 0);
		}

		bool DX_CommandList::HasCommands() {
			return command_list_dirty;
		}
	}
}