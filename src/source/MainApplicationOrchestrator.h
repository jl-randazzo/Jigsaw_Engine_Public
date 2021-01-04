#ifndef _MAIN_APPLICATION_ORCHESTRATION_H_
#define _MAIN_APPLICATION_ORCHESTRATION_H_

#include "Graphics/DirectX/DX_Context.h"
#include "Windows/ViewportWindow.h"
#include "Graphics/Pipeline/PipelineService.h"
#include "Graphics/RenderContext.h"
#include "Camera.h"
#include "Physics/Transform.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Entities/JigsawEntityClusterService.h"
#include "Orchestration/JigsawSystemsOrchestrator.h"
#include "Orchestration/SceneHierarchy.h"

namespace Jigsaw {
	namespace Orchestration {
		/// <summary>
		/// </summary>
		class MainApplicationOrchestrator {

		public:
			static MainApplicationOrchestrator* GetOrchestrator(Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context, Jigsaw::Ref<ViewportWindow> main_window);

			void Orchestrate();

		private:
			MainApplicationOrchestrator(Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context, Jigsaw::Ref<ViewportWindow> main_window);

			Jigsaw::System::THREAD_SAFE_RESOURCE_POOL resource_pool;

			const Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context;
			const Jigsaw::Ref<ViewportWindow> main_window;
			Jigsaw::Pipeline::PipelineServiceManager& ps_manager;
			Jigsaw::Ref<Jigsaw::Entities::JigsawEntityClusterService> cluster_service;
			Jigsaw::Ref<Jigsaw::Orchestration::JigsawSystemsOrchestrator> systems_orchestrator;
			Jigsaw::Ref<Jigsaw::Orchestration::SceneHierarchy> scene_hierarchy;

		};
	}
}

#endif

