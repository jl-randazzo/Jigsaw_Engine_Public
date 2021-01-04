#pragma once
#include "Assets/AssetManagement.h"
#include <map>
#include <vector>
#include "Graphics/__graphics_api.h"
#include "Graphics/GPURuntimeObjects.h"

#define PSO_RAW_STREAM_INITIAL_BYTES 256 

namespace Jigsaw {
	namespace Pipeline {
		/// <summary>
		/// PipelineObjectBuilder is a simple interface used primarily by the PipelineServiceManager to quickly build pipelines without concerning the user about 
		/// creating custom Structs. All of the Stream Subobjects are placed directly in void-aligned memory
		/// </summary>
		/// <author>
		///  Luke Randazzo
		/// </author>
		class PipelineObjectBuilder {
		public:
			PipelineObjectBuilder(); 

			~PipelineObjectBuilder();

			/// <summary>
			/// Adds a shader of any type to the Pipeline State Object
			/// </summary>
			/// <param name="shader"></param>
			/// <returns></returns>
			PipelineObjectBuilder& AddShader(const Jigsaw::Assets::ShaderResource* shader);

			/// <summary>
			/// Pushes a single element to the list of Input Assembler Vertex data
			/// </summary>
			/// <param name="semantic_name"></param>
			/// <param name="format"></param>
			/// <returns></returns>
			PipelineObjectBuilder& IAPushPerVertexDesc(LPCSTR semantic_name, const DXGI_FORMAT format);

			/// <summary>
			/// Pushes an output parameter to be exposed in Stream Output stage
			/// </summary>
			/// <param name="semantic_name"></param>
			/// <param name="component_count"></param>
			/// <param name="output_slot"></param>
			/// <returns></returns>
			PipelineObjectBuilder& SOPushOutputParameter(LPCSTR semantic_name, const UINT semantic_index, const UINT component_count, const UINT output_slot);

			/// <summary>
			/// Sets the root signature for the pipeline
			/// </summary>
			/// <param name="rs_resource"></param>
			/// <returns></returns>
			PipelineObjectBuilder& RSSetRootSignature(const Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject>& rs_resource);

			/// <summary>
			/// Increments the count of render targets and binds the format to the added target
			/// </summary>
			/// <param name="format"></param>
			/// <returns></returns>
			PipelineObjectBuilder& RTVAddTargetFormat(DXGI_FORMAT format);

			/// <summary>
			/// Sets the format for the Depth stencil view
			/// </summary>
			/// <param name="format"></param>
			/// <returns></returns>
			PipelineObjectBuilder& DSVSetFormat(DXGI_FORMAT format);

			/// <summary>
			/// Primitive topology type for PSO set here
			/// </summary>
			/// <param name="topology_type"></param>
			/// <returns></returns>
			PipelineObjectBuilder& SetPrimitiveTopology(Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE topology_type);

			/// <summary>
			/// Returns a fully-constructed pipeline state object. This is the final step. All of the other methods should be called prior
			/// to the final 'Build' call. If the arguments were ill-formed, an exception will be thrown. 
			/// </summary>
			/// <param name="cmd_list"></param>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::Graphics::PipelineObject> Build(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list);

		private:

			/// <summary>
			/// Internal method for accumulating any Pipeline State Subobject into the raw data stream. The stream will resize automatically as needed.
			/// </summary>
			/// <typeparam name="D"></typeparam>
			/// <typeparam name="I"></typeparam>
			/// <param name="subobject"></param>
			template <typename I, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename D = I>
			void PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT<I, Type, D> subobject) {

				size_t size_in_bytes = sizeof(CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT<I, Type, D>);
				// check if we need to resize
				if ((data_size + size_in_bytes) > data_capacity) {

					do {
						data_capacity += PSO_RAW_STREAM_INITIAL_BYTES;
					} while ((data_size + size_in_bytes) > data_capacity);

					void* old_data_stream = raw_data_stream;
					raw_data_stream = new char[data_capacity];

					memcpy(raw_data_stream, old_data_stream, data_size);
					delete[] old_data_stream;
				}

				UINT* start_ptr = static_cast<UINT*>(raw_data_stream) + (data_size / sizeof(UINT));
				*start_ptr = Type;
				start_ptr += (sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE) / sizeof(UINT));

				I sub = (I)subobject;
				memcpy(start_ptr, &sub, size_in_bytes - sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE));
				data_size += size_in_bytes;
			}

			std::map<LPCSTR, UINT> semantic_map;
			std::vector<D3D12_INPUT_ELEMENT_DESC> ia_desc;
			std::vector<D3D12_SO_DECLARATION_ENTRY> so_decl;
			std::vector<UINT> so_decl_strides;

			Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE topology_type;

			D3D12_RT_FORMAT_ARRAY rt_formats;
			Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> rs_object;

			size_t data_capacity;
			UINT data_size;
			void* raw_data_stream;
		};
	}
}
