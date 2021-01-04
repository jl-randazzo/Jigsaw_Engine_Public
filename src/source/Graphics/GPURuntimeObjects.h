#ifndef _GPU_RUNTIME_OBJECTS_H
#define _GPU_RUNTIME_OBJECTS_H

#include "Graphics/__graphics_api.h"
#include "Assets/RuntimeAssets.h"
#include "Ref.h"

/// <summary>
/// GPURuntimeObjects encompass many different object types that are distinct from API implementation to implementation,
/// but from the perspective of client code, they are not materially different. 
/// </summary>
namespace Jigsaw {
	namespace Graphics {
		/// <summary>
		/// Generic Topology type object
		/// </summary>
		enum JGSW_TOPOLOGY_TYPE {
			JGSW_TOPOLOGY_TYPE_UNDEFINED = 0,
			JGSW_TOPOLOGY_TYPE_POINT = 1,
			JGSW_TOPOLOGY_TYPE_LINE = 2,
			JGSW_TOPOLOGY_TYPE_TRIANGLE = 3,
			JGSW_TOPOLOGY_TYPE_PATCH = 4
		};

		/// <summary>
		/// Interface for RootSignatureObjects--these can be abstractions used to orchestrate drawing or real objects on the GPU
		/// </summary>
		class RootSignatureObject {
		public:
			RootSignatureObject() {};

#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature();
#endif

		protected:

			size_t size;

#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature;
#endif

		};

		/// <summary>
		/// Interface for PipelineObjects. The RootSignature is also associated
		/// </summary>
		class PipelineObject {
		public:
			PipelineObject(const Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject>& rs_object, JGSW_TOPOLOGY_TYPE topology_type) 
				: rs_object(rs_object), topology_type(topology_type) {}

			JGSW_TOPOLOGY_TYPE GetTopologyType() const;

#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPipelineState();
			Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature();
#endif
		protected:

			Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> rs_object;

#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12PipelineState> ps_object;
#endif
			JGSW_TOPOLOGY_TYPE topology_type;

		};

		/// <summary>
		/// Class is designed for multiple inheritance with another Resource class, likely RuntimeAsset
		/// </summary>
		class GPUBuffer {
		public:
			GPUBuffer(size_t size, size_t element_size) : size(size), element_size(element_size) {}

			size_t GetSize() const;

			size_t GetElementSize() const;

#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12Resource>& GetResourceReference() {
				return dx_buffer;
			}

			const Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const {
				return dx_buffer;
			}

			Microsoft::WRL::ComPtr<ID3D12Resource>& GetCopyResource() {
				return dx_copy_buffer;
			}

			void ReleaseCopyResource() {
				while (dx_copy_buffer->Release()) {}
			}

			/// <summary>
			/// Transition method moves the state of the current buffer resource and adjusts the state internally
			/// </summary>
			/// <param name="cmd_list"></param>
			/// <param name="next_state"></param>
			void Transition(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmd_list, D3D12_RESOURCE_STATES next_state) {
				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx_buffer.Get(), resource_state, next_state);
				resource_state = next_state;
				cmd_list->ResourceBarrier(1, &barrier);
			}
#endif
		protected:
#ifdef _J_WIN_10_
			Microsoft::WRL::ComPtr<ID3D12Resource> dx_buffer;
			Microsoft::WRL::ComPtr<ID3D12Resource> dx_copy_buffer;
#endif
			const size_t size;
			const size_t element_size;

			D3D12_RESOURCE_STATES resource_state;
		};

	}
}


#endif