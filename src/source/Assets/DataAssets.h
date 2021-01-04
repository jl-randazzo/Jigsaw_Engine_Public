#ifndef _ASSET_RESOURCES_H_
#define _ASSET_RESOURCES_H_

#include "BaseAsset.h"
#include "Graphics/DirectX/DX_Context.h"
#include <d3d12.h>
#include <d3d11_2.h>
#include "Graphics/DirectX/DirectXDataLayouts.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Ref.h"
#include "Util/etype_info.h"
#include "File_IO/FileUtil.h"
#include "Graphics/CommandList.h"
#include "Graphics/Pipeline/JigsawPipeline.h"
#include "Debug/j_debug.h"

namespace Jigsaw {
	namespace Assets {

		/**
		 * DataAssets are read from the hard drive at runtime. They are shareable. This includes everything from Models to Shaders to Json.
		 */
		class DataAsset : public BaseAsset {
		public:
			friend Jigsaw::Ref<DataAsset> FetchAsset(const char* file_name, ASSET_TYPE type, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);
			friend Jigsaw::Unique<Jigsaw::Ref<DataAsset>[]> FetchAssets(const Jigsaw::Unique<Jigsaw::System::UID[]>& uids, const int count, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);
			friend void Init(Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

			virtual ~DataAsset() { }

		protected:
			DataAsset(const AssetDescriptor& descriptor) : BaseAsset(descriptor) { }

			// these private, virtual implementations are designed to be used by privileged resource management functions
			// the resource management context will query a table for the availability of certain resources in memory or load them if they are absent. 
			virtual ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) = 0;

		};

		class JsonAsset : public DataAsset {
		public:
			JsonAsset(const AssetDescriptor& descriptor) : DataAsset(descriptor), t_info(Jigsaw::Util::etype_info::GetByQualifiedName(descriptor.fully_qualified_type_name)) {}

			~JsonAsset() {
				if (object) {
					bool delete_res = t_info.GetUnsafeFunctions().Delete(object);
					J_D_ASSERT_LOG_ERROR(delete_res, JsonAsset, "Failed to delete JsonAsset object");
				}
			}

			void* GetRawData();

			const Jigsaw::Util::etype_info& GetTypeInfo();

		protected:
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);
			void* object = nullptr;
			const Jigsaw::Util::etype_info& t_info;

		};

		/// <summary>
		/// Special wrapper class for accessing the underlying data of a JsonAsset with the type specified.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template<typename T>
		class SerializedRef {
		public:
			/// <summary>
			/// Default constructor
			/// </summary>
			SerializedRef() : type_info(Jigsaw::Util::etype_info::Id<T>()), json_asset() {}

			/// <summary>
			/// Copy constructor
			/// </summary>
			/// <param name="other"></param>
			SerializedRef(const SerializedRef& other) : json_asset(other.json_asset), type_info(Jigsaw::Util::etype_info::Id<T>()) {}

			/// <summary>
			/// Primary constructor used to initially assign a json_asset
			/// </summary>
			/// <param name="json_asset"></param>
			SerializedRef(Jigsaw::Ref<JsonAsset>& json_asset) : json_asset(json_asset), type_info(Jigsaw::Util::etype_info::Id<T>()) {
				bool eq = Jigsaw::Util::etype_index(json_asset->GetTypeInfo()) == Jigsaw::Util::etype_index(this->type_info);
				J_D_ASSERT_LOG_ERROR(eq, SerializedRef, "A serialized reference was attempted to be initialized with a non-matching type");
			}

			SerializedRef& operator=(Ref<JsonAsset>& asset) {
				json_asset = asset;
				return *this;
			}

			/// <summary>
			/// Move assignment is leveraged by the marshalling context 
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			SerializedRef& operator=(SerializedRef<T>&& other) {
				json_asset = other.json_asset;
				return *this;
			}

			/// <summary>
			/// Gets the underlying asset that this template obscures
			/// </summary>
			/// <returns></returns>
			Jigsaw::Ref<JsonAsset>& GetJsonAsset() {
				return json_asset;
			}

			/// <summary>
			/// Access the underlying object in the json asset.
			/// </summary>
			/// <returns></returns>
			T* operator->() const {
				return static_cast<T*>(json_asset->GetRawData());
			}

			/// <summary>
			/// Releases the underlying asset reference
			/// </summary>
			void Reset() {
				json_asset.reset();
			}

		private:
			Jigsaw::Ref<JsonAsset> json_asset;
			const Jigsaw::Util::etype_info& type_info;

		};

		/**
		* ModelResources can read from .obj file formats (perhaps others as well)
		*/
		class ModelResource : public BaseAsset {
		public:
			ModelResource(const AssetDescriptor& descriptor) : BaseAsset(descriptor) {}
			ID3D11Buffer const* GetVertexBuffer();
			ID3D11Buffer const* GetIndexBuffer();

			~ModelResource() {
				while (vert_buffer->Release()) {}
				while (ind_buffer->Release()) {}
			}
		protected:

			Microsoft::WRL::ComPtr<ID3D11Buffer> vert_buffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> ind_buffer;

			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);
		};

		/**
		* VertexShaders
		*/
		class ShaderResource : public DataAsset {
		public:
			ShaderResource(const AssetDescriptor& descriptor) : DataAsset(descriptor) { }

			ID3DBlob* GetShader() const {
				return shader.Get();
			}

			const D3D12_SHADER_BYTECODE GetShaderBytes() const {
				return bytecode_desc;
			}

			~ShaderResource();

		protected:
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

			D3D12_SHADER_BYTECODE bytecode_desc;
			Microsoft::WRL::ComPtr<ID3DBlob> shader;
		};

		/**
		* VertexShaders
		*/
		class VertexShaderResource : public DataAsset {
		public:
			VertexShaderResource(const AssetDescriptor& descriptor) : DataAsset(descriptor) { }
			ID3DBlob* GetShader() const {
				return vertex_shader.Get();
			}

			const D3D12_SHADER_BYTECODE GetShaderBytes() const {
				return bytecode_desc;
			}

			~VertexShaderResource();

		protected:
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

			D3D12_SHADER_BYTECODE bytecode_desc;
			Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader;
		};

		/**
		* Geometry shader, can be mocked from a vertex shader if the goal is to use it with the SO stage
		*/
		class GeometryShaderResource : public DataAsset {
		public:
			friend GeometryShaderResource* CreateMockGeometryShaderForStreamOutput(const VertexShaderResource& vertex_shader,
				const D3D11_SO_DECLARATION_ENTRY* so_declarations, const size_t decl_count, const size_t stride);

			GeometryShaderResource(const AssetDescriptor& descriptor) : DataAsset(descriptor), bytecode_desc() {}

			ID3DBlob* GetShader() {
				return geometry_shader.Get();
			}

			~GeometryShaderResource();

		protected:
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

			D3D12_SHADER_BYTECODE bytecode_desc;
			Microsoft::WRL::ComPtr<ID3DBlob> geometry_shader;
		};

		/**
		* PixelShaders
		*/
		class PixelShaderResource : public DataAsset {
		public:
			PixelShaderResource(const AssetDescriptor& descriptor) : DataAsset(descriptor) { }
			ID3DBlob* GetShader() const {
				return pixel_shader.Get();
			}

		protected:
			~PixelShaderResource();
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources);

			D3D12_SHADER_BYTECODE bytecode_desc;
			Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader;
		};

		class Cube : public DataAsset {
		public:
			Cube(const AssetDescriptor& descriptor, size_t s) : DataAsset(descriptor), scale(s), ind_array_size(0) { }

			~Cube() {
			}

			/**
			* Returns a struct describing the vertex buffer view
			*/
			D3D12_VERTEX_BUFFER_VIEW GetVBV() {
				D3D12_VERTEX_BUFFER_VIEW vbv;
				vbv.BufferLocation = vert_buffer_->GetResource()->GetGPUVirtualAddress();
				vbv.SizeInBytes = vbuff_size;
				vbv.StrideInBytes = sizeof(PositionColorNormal);
				return vbv;
			}

			/**
			* Returns a struct describing the location, format, and size of the index buffer on the GPU
			*/
			D3D12_INDEX_BUFFER_VIEW GetIBV() {
				D3D12_INDEX_BUFFER_VIEW ibv;
				ibv.BufferLocation = ind_buffer_->GetResource()->GetGPUVirtualAddress();
				ibv.Format = DXGI_FORMAT_R32_UINT;
				ibv.SizeInBytes = ind_array_size;
				return ibv;
			}

			Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>& GetBuffer() {
				return vert_buffer_;
			}

			Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>& GetIndexBuffer(size_t* size) {
				*size = ind_array_size;
				return ind_buffer_;
			}

		private:
			ASSET_LOAD_RESULT Load(const Jigsaw::File_IO::FILE_DATA& file_data, Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources) {
				PositionColorNormal pos_col_array[]{
					{ Vector3(-1, -1, -1), Vector3(1, 0, 0) , Vector3(0, 0, -1) }, { Vector3(1, -1, -1), Vector3(1, 1, 0), Vector3(0, 0, -1) },
					{ Vector3(-1, 1, -1), Vector3(0, 1, 1), Vector3(0, 0, -1) }, { Vector3(1, 1, -1), Vector3(0, 1, 1), Vector3(0, 0, -1) },
				{ Vector3(1, -1, -1), Vector3(1, 1, 0), Vector3(1, 0, 0) }, { Vector3(1, 1, 1), Vector3(1, 1, 0), Vector3(1, 0, 0) },
				{ Vector3(1, 1, -1), Vector3(0, 1, 1), Vector3(1, 0, 0) }, { Vector3(1, -1, 1), Vector3(1, 1, 0), Vector3(1, 0, 0) },
				{ Vector3(-1, -1, 1), Vector3(1, 0, 0), Vector3(0, 0, 1) }, { Vector3(-1, 1, 1), Vector3(1, 0, 0), Vector3(0, 0, 1), },
				{ Vector3(1, 1, 1), Vector3(1, 1, 0), Vector3(0, 0, 1) }, { Vector3(1, -1, 1), Vector3(1, 1, 0), Vector3(0, 0, 1) },
				{ Vector3(-1, -1, -1), Vector3(1, 0, 0), Vector3(0, -1, 0) }, { Vector3(1, -1, 1), Vector3(1, 1, 0), Vector3(0, -1, 0) },
				{ Vector3(1, -1, -1), Vector3(1, 1, 0), Vector3(0, -1, 0) }, { Vector3(-1, -1, 1), Vector3(1, 0, 0), Vector3(0, -1, 0) },
				{ Vector3(-1, 1, -1), Vector3(0, 1, 1), Vector3(-1, 0, 0) }, { Vector3(-1, -1, 1), Vector3(1, 0, 0), Vector3(-1, 0, 0) },
				{ Vector3(-1, -1, -1), Vector3(1, 0, 0), Vector3(-1, 0, 0) }, { Vector3(-1, 1, 1), Vector3(1, 0, 0), Vector3(-1, 0, 0) },
				{ Vector3(-1, 1, 1), Vector3(1, 0, 0), Vector3(0, 1, 0) }, { Vector3(-1, 1, -1), Vector3(0, 1, 1), Vector3(0, 1, 0) },
				{ Vector3(1, 1, -1), Vector3(0, 1, 1), Vector3(0, 1, 0) }, { Vector3(1, 1, 1), Vector3(1, 1, 0), Vector3(0, 1, 0) }
				};

				UINT ind_array[]{
					1, 3, 0, // bottom face
					0, 3, 2,
					4, 5, 6, // back face
					4, 7, 5,
					8, 9, 10, // top face
					8, 10, 11,
					12, 13, 14, // right face
					12, 15, 13,
					16, 17, 18, // front face
					16, 19, 17,
					20, 21, 22, // left face
					20, 22, 23
				};

				sys_resources.cmd_list->LoadBuffer((BYTE*)pos_col_array, sizeof(PositionColorNormal), ARRAYSIZE(pos_col_array), &vert_buffer_);

				sys_resources.cmd_list->LoadBuffer((BYTE*)ind_array, sizeof(UINT), ARRAYSIZE(ind_array), &ind_buffer_);

				ASSET_LOAD_RESULT result;
				result.result = COMPLETE;

				return result;
			}

			Jigsaw::Pipeline::JIGSAW_PIPELINE pipeline;
			Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer> vert_buffer_, ind_buffer_;
			size_t ind_array_size;
			size_t vbuff_size;
			size_t scale;
		};

	}
}
#endif // !_ASSET_RESOURCES_H_
