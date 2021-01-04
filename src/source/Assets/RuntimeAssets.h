#ifndef _RUNTIME_RESOURCES_H_
#define _RUNTIME_RESOURCES_H_
#include <unordered_map>
#include "BaseAsset.h"
#include <d3d12.h>
#undef UpdateResource

namespace Jigsaw {
	namespace Assets {

		/**
		* Runtime resources are non-fetchable resources. They are meant to be constant buffers or other resources that are not shared
		* and not serialized. They are built at runtime and not loaded from a data segment or stored as a key in the database.
		* The owner is responsible for calling releaseResources with a reference to the runtime subresource to remove it.
		*/
		template <typename T>
		class RuntimeAsset : virtual public BaseAsset {
		public:
			friend AssetDescriptor UpdateResource(const RuntimeAsset<T>* resource, const T* newData, size_t count, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd_list);
			template <typename T>
			friend RuntimeAsset<T>* CreateResource(const T* data, size_t count, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd_list);

			RuntimeAsset<T>(const AssetDescriptor& descriptor) : BaseAsset(descriptor) { }

		protected:
			virtual void Initialize(const T* data, const size_t size) = 0;
			virtual void Update(const T* data, const size_t size) = 0;
		};


		/**
		* Input layout resource descriptor. Maintains a map of InputLayouts corresponding to Vertex Shaders.
		*/
		/*
		class InLayoutResource : public RuntimeAsset<D3D12_INPUT_ELEMENT_DESC>, public GPUBufferResource {
		public:
			InLayoutResource(const AssetDescriptor& descriptor) : RuntimeAsset<D3D12_INPUT_ELEMENT_DESC>(descriptor),
				GPUBufferResource(descriptor), BaseAsset(descriptor), data(NULL), size(0) {}
			ID3D11InputLayout* GetLayout(const VertexShaderResource& shader_resource);

		protected:
			void Initialize(const D3D12_INPUT_ELEMENT_DESC* data, const size_t size);
			void Update(const D3D12_INPUT_ELEMENT_DESC* data, const size_t update_size);

			~InLayoutResource() {
				delete[] data;
			}

		private:
			std::unordered_map<const ID3D11VertexShader*, Microsoft::WRL::ComPtr<ID3D11InputLayout>> vshader_layout_map;
			// this is a copy of the data that were passed in
			D3D12_INPUT_ELEMENT_DESC* data;
			size_t size;
		};

		class RSResource : public RuntimeAsset<D3D11_RASTERIZER_DESC> {
		public:
			RSResource(const AssetDescriptor& descriptor) : RuntimeAsset<D3D11_RASTERIZER_DESC>(descriptor), BaseAsset(descriptor) {}
			ID3D11RasterizerState* GetRasterizerState() const {
				return rasterizer_state.Get();
			}

		protected:
			void Initialize(const D3D11_RASTERIZER_DESC* data, const size_t size);
			void Update(const D3D11_RASTERIZER_DESC* data, const size_t update_size);

			~RSResource() {}

		private:
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;
		};

		enum COPY_BUFFER_TYPE {
			GPU_UPLOAD, GPU_DOWNLOAD
		};

		struct SO_DESC {
			size_t size;
		};

		/**
		 * Stream output buffer is used either to capture important outputs from the geometry shader stage or to debug
		class SOBufferResource : public RuntimeAsset<SO_DESC>, public GPUBufferResource {
		public:
			SOBufferResource(const AssetDescriptor& descriptor) : RuntimeAsset<SO_DESC>(descriptor), GPUBufferResource(descriptor),
				BaseAsset(descriptor) {}
			D3D12_STREAM_OUTPUT_BUFFER_VIEW GetSOBV() const;

		protected:
			void Initialize(const SO_DESC* data, const size_t size);
			void Update(const SO_DESC* data, const size_t size);
			size_t size;
		};

		struct COPY_DESC {
			size_t size;
			COPY_BUFFER_TYPE type;
		};
		 */

		/// <summary>
		/// Interface for RootSignature Resources on the GPU
		/// </summary>
		/*
		class RootSignatureResource : public RuntimeAsset<CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC> {
		public:
			RootSignatureResource(const AssetDescriptor& descriptor) : RuntimeAsset<CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC>(descriptor), BaseAsset(descriptor) {}
			ID3D12RootSignature* GetRootSignature() const;

		protected:
			void Initialize(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC* data, const size_t size);
			void Update(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC* data, const size_t size);
			size_t size;

			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature;

		};
		/*/

	}
}
#endif
