#pragma once
#include "Graphics/RenderContext.h"
#include "RootSignatureBuilder.h"
#include "Assets/DataAssets.h"
#include "JigsawPipeline.h"

#define JGSW_PIPELINE_DRAW_UID_STRING "f3a844dd-07bb-4791-8542-693efcad11d0"
#define JGSW_PIPELINE_DRAW_UID UIDFromString(JGSW_PIPELINE_DRAW_UID_STRING)

namespace Jigsaw {
	namespace Orchestration {
		class MainApplicationOrchestrator;
	}
}

namespace Jigsaw {
	namespace Pipeline {

		/// <summary>
		/// The PipelineServiceManager is in charge of delivering PipelineStateResources, Shaders, Root signature objects,
		/// and any other graphics-pipeline-related tools needed by the MainApplicationOrchestrator or a renderer to execute a draw function on the GPU
		///
		/// This class requires resources allocated by the DX_Context and passed by the MainApplicationOrchestrator to initialize PipelineStateResources, ShaderResources, 
		/// Input layouts, Rasterization stages, stenciling pipelines and more.
		/// </summary>
		class PipelineServiceManager {
		public:
			// access to the MainApplicationOrchestrator is granted to make global changes to the PipelineService instance if necessary
			friend class Jigsaw::Orchestration::MainApplicationOrchestrator;

			/// <summary>
			/// The 'GetPipeline' function is publicly available. It is assumed the MainApplicationOrchestrator has initialized the PiplineService
			///This will return a PipelineStateResource that can be used to initialize the PS/RS-related steps of the GraphicsCommandList
			/// </summary>
			/// <param name="pipeline"></param>
			/// <returns></returns>
			static const Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& GetPipeline(JIGSAW_PIPELINE pipeline);

		private:
			// The pipeline service instance is only available to the PipelineServiceManager's static functions and the MainApplicationOrchestrator
			static PipelineServiceManager& GetPipelineService();

			PipelineServiceManager() : pipelines(), shaders() {}

			// this method handles the initial construction of the PipelineServiceManager, and it is called by the MainApplicationOrchetrator upon construction
			HRESULT InitializePipelines(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list);

			std::map<JIGSAW_PIPELINE, Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>> pipelines;
			std::map<JIGSAW_PIPELINE, Jigsaw::Ref<Jigsaw::Assets::ShaderResource>> shaders;

		};
	}
}
