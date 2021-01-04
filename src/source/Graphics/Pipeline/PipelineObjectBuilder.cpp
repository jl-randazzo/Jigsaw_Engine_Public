#include "PipelineObjectBuilder.h"
#include "Graphics/__graphics_api_context.h"
#include "Debug/j_debug.h"

using namespace Jigsaw::Assets;

namespace Jigsaw {
	namespace Pipeline {

#ifdef _J_WIN_10_
		PipelineObjectBuilder::PipelineObjectBuilder() : raw_data_stream(new void* [PSO_RAW_STREAM_INITIAL_BYTES]), data_capacity(PSO_RAW_STREAM_INITIAL_BYTES),
			data_size(0), rt_formats({}) {
			rt_formats.NumRenderTargets = 0;
			ZeroMemory(raw_data_stream, PSO_RAW_STREAM_INITIAL_BYTES);
		}

		PipelineObjectBuilder::~PipelineObjectBuilder() {
			delete[] raw_data_stream;
		}
#endif

		PipelineObjectBuilder& PipelineObjectBuilder::AddShader(const Jigsaw::Assets::ShaderResource* shader) {

			switch (shader->GetDescriptor().type) {
			case ASSET_VERTEX_SHADER:
				PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_VS(shader->GetShaderBytes()));
				break;
			case ASSET_PIXEL_SHADER:
				PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_PS(shader->GetShaderBytes()));
				break;
			case ASSET_GEOMETRY_SHADER:
			default:
				PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_GS(shader->GetShaderBytes()));
				break;
			}

			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::IAPushPerVertexDesc(LPCSTR semantic_name, const DXGI_FORMAT format) {
			auto it = semantic_map.find(semantic_name);
			UINT semantic_index = 0;
			if (it != semantic_map.end()) {
				it->second += 1;
				semantic_index = it->second;
			}
			else {
				semantic_map.insert(std::make_pair(semantic_name, 0));
			}

			D3D12_INPUT_ELEMENT_DESC ia_desc_element =
			{ semantic_name, semantic_index, format,
				0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			ia_desc.push_back(ia_desc_element);

			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::RTVAddTargetFormat(DXGI_FORMAT format) {
			rt_formats.RTFormats[rt_formats.NumRenderTargets++] = format;
			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::DSVSetFormat(DXGI_FORMAT format) {
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat = format;
			PushStreamSubObject(DSVFormat);
			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::SOPushOutputParameter(LPCSTR semantic_name, const UINT semantic_index, const UINT component_count, const UINT output_slot) {
			D3D12_SO_DECLARATION_ENTRY so_decl;
			so_decl.Stream = 0;
			so_decl.SemanticName = semantic_name;
			so_decl.SemanticIndex = semantic_index;
			so_decl.StartComponent = 0;
			so_decl.ComponentCount = component_count;
			so_decl.OutputSlot = output_slot;
			this->so_decl.push_back(so_decl);

			so_decl_strides.push_back(component_count * 4);

			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::RSSetRootSignature(const Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject>& rs_object) {
			this->rs_object = rs_object;
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE root_sig = rs_object->GetRootSignature().Get();
			PushStreamSubObject(root_sig);
			return *this;
		}

		PipelineObjectBuilder& PipelineObjectBuilder::SetPrimitiveTopology(Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE topology_type) {
			this->topology_type = topology_type;
			PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY((D3D12_PRIMITIVE_TOPOLOGY_TYPE)topology_type));
			return *this;
		}

		Jigsaw::Ref<Jigsaw::Graphics::PipelineObject> PipelineObjectBuilder::Build(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list) {
			Jigsaw::Ref<Jigsaw::Graphics::PipelineObject> pl_object = Jigsaw::MakeRef<Jigsaw::Graphics::PipelineObject>(rs_object, topology_type);
#ifdef _J_WIN_10_
			// compiling the Stream Output declarations into the PSO
			if (!so_decl.empty()) {
				D3D12_SO_DECLARATION_ENTRY* so_decl_array = so_decl.data();
				UINT count = so_decl.size();
				UINT* strides = so_decl_strides.data();

				D3D12_STREAM_OUTPUT_DESC so_desc;
				so_desc.NumEntries = count;
				so_desc.pBufferStrides = strides;
				so_desc.NumStrides = count;
				so_desc.pSODeclaration = so_decl_array;
				so_desc.RasterizedStream = 0;
				PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_STREAM_OUTPUT(so_desc));
			}

			// compiling the Input Assembly descriptions into the PSO
			if (!ia_desc.empty()) {
				D3D12_INPUT_ELEMENT_DESC* desc_array = ia_desc.data();
				UINT count = ia_desc.size();
				D3D12_INPUT_LAYOUT_DESC layout_desc = { desc_array, count };

				CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT input_layout = layout_desc;
				PushStreamSubObject(input_layout);
			}

			// pushing the formats onto the subobject stream
			PushStreamSubObject(CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS(rt_formats));

			// fetching the dx12 device object from the command list
			Jigsaw::Ref<Jigsaw::Graphics::DX_CommandList> dx_cmd_list = std::dynamic_pointer_cast<Jigsaw::Graphics::DX_CommandList>(cmd_list);
			Microsoft::WRL::ComPtr<ID3D12Device2> device;
			dx_cmd_list->GetDXCMDList()->GetDevice(IID_PPV_ARGS(&device));
		
			D3D12_PIPELINE_STATE_STREAM_DESC stream_desc{
				data_size - 1, raw_data_stream
			};

			HRESULT pso_result = device->CreatePipelineState(&stream_desc, __uuidof(ID3D12PipelineState), &pl_object->GetPipelineState());
			//Jigsaw::Graphics::DX_Context::PrintDebugMessages();
			J_D_ASSERT_LOG_ERROR(!FAILED(pso_result), PipelineObjectBuilder, "Failed to create PSO");
#endif
			return pl_object;
		}
	}
}
