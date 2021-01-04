#include "PipelineService.h"
#include "Graphics/Pipeline/PipelineObjectBuilder.h"
#include "Graphics/__graphics_api_context.h"
#include "Graphics/GPURuntimeObjects.h"

using namespace Jigsaw::Assets;
using namespace Jigsaw::System;

namespace Jigsaw {
	namespace Pipeline {
		PipelineServiceManager& PipelineServiceManager::GetPipelineService() {
			static PipelineServiceManager* instance = new PipelineServiceManager;
			return *instance;
		}

		const Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& PipelineServiceManager::GetPipeline(JIGSAW_PIPELINE pipeline) {
			return GetPipelineService().pipelines.at(pipeline);
		}

		Ref<Jigsaw::Assets::ShaderResource> vert_shader_resource;
		Ref<Jigsaw::Assets::ShaderResource> pix_shader_resource;
		HRESULT PipelineServiceManager::InitializePipelines(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list) {
			THREAD_SAFE_SYSTEM_RESOURCES sys_resources;
			sys_resources.cmd_list = cmd_list;
			vert_shader_resource = std::dynamic_pointer_cast<ShaderResource>(FetchAsset(".\\Debug\\BasicVertex.cso", ASSET_VERTEX_SHADER, sys_resources));
			pix_shader_resource = std::dynamic_pointer_cast<ShaderResource>(FetchAsset(".\\Debug\\BasicPixel.cso", ASSET_PIXEL_SHADER, sys_resources));

			RootSignatureBuilder rs_builder;
			rs_builder.SetNextShaderVisiblity(D3D12_SHADER_VISIBILITY_VERTEX)
				.PushStructAs32BitConstant<MVPMatrixConstant>(0)
				.SetNextShaderVisiblity(D3D12_SHADER_VISIBILITY_PIXEL)
				.PushStructAs32BitConstant<WorldLight>(0)
				.OrRootFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
			Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> rs_object = rs_builder.Build(cmd_list);

			PipelineObjectBuilder ps_builder;
			ps_builder.IAPushPerVertexDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
				.IAPushPerVertexDesc("COLOR", DXGI_FORMAT_R32G32B32_FLOAT)
				.IAPushPerVertexDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT)
				.SOPushOutputParameter("NORMAL", 0, 4, 0)
				.RSSetRootSignature(rs_object)
				.AddShader(vert_shader_resource.get()).AddShader(pix_shader_resource.get())
				.SetPrimitiveTopology(Jigsaw::Graphics::JGSW_TOPOLOGY_TYPE_LINE)
				.DSVSetFormat(DXGI_FORMAT_D24_UNORM_S8_UINT).RTVAddTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			Jigsaw::Ref<Jigsaw::Graphics::PipelineObject> ps_resource = ps_builder.Build(cmd_list);

			pipelines[JIGSAW_PIPELINE_DRAW] = ps_resource;

			return 0;
		}
	}
}
