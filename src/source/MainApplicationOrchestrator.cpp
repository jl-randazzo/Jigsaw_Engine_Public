#include "Ref.h"
#include "System/THREAD_SAFE_SYSTEM_RESOURCES.h"
#include "Application/ApplicationRootProperties.h"
#include "Marshalling/MarshallingUtil.h"
#include "Debug/j_log.h"
#include "MainApplicationOrchestrator.h"
#include "Graphics/Renderer.h"

// initializers 
using namespace Microsoft::WRL;
using namespace Jigsaw::Assets;
using namespace Jigsaw::System;

namespace Jigsaw {
	namespace Orchestration {
		MainApplicationOrchestrator* MainApplicationOrchestrator::GetOrchestrator(Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context, Jigsaw::Ref<ViewportWindow> main_window) {
			static MainApplicationOrchestrator*	orchestrator = new MainApplicationOrchestrator(render_context, main_window);
			return orchestrator;
		}

		int i = 0;
		//SOBufferResource* so_buffer;
		//CopyBufferResource* copy_buffer;
		Transform* test_transform = new Transform();
		Camera* camera = new Camera(Vector3(0, 0, 0), Vector3(1, 0, 0));
		MainApplicationOrchestrator::MainApplicationOrchestrator(Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context, Jigsaw::Ref<ViewportWindow> main_window)
			: main_window(main_window), render_context(render_context), ps_manager(Jigsaw::Pipeline::PipelineServiceManager::GetPipelineService()) {

			test_transform->position = Vector3(5, 0, 0);
			Jigsaw::System::THREAD_SAFE_SYSTEM_RESOURCES sys_resources;
			sys_resources.db = Jigsaw::MakeRef <Jigsaw::Assets::AssetDatabaseReader>(ApplicationRootProperties::Get().db_connection.c_str());
			sys_resources.cmd_list = render_context->GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD);
			auto cmd_list_exec = render_context->GetCommandListExecutor(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_LOAD);

			ps_manager.InitializePipelines(sys_resources.cmd_list);

			cmd_list_exec->SubmitCommandList(sys_resources.cmd_list);
			cmd_list_exec->SignalAndWait();

			cluster_service = Jigsaw::MakeRef<Jigsaw::Entities::JigsawEntityClusterService>();
			systems_orchestrator = Jigsaw::MakeRef<Jigsaw::Orchestration::JigsawSystemsOrchestrator>(cluster_service);
			scene_hierarchy = Jigsaw::Orchestration::SceneHierarchy::Init(render_context, cluster_service);

			scene_hierarchy->LoadAndActivateScene(ApplicationRootProperties::Get().root_scene_id);
		}

		/**
		* The primary call orchestrates calls between all primary systems in the application world
		*/
		void MainApplicationOrchestrator::Orchestrate() {
			render_context->StartFrame();

			auto cmd_list = render_context->GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_RENDER);
			auto cmd_list_exec = render_context->GetCommandListExecutor(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE_RENDER);
			Renderer::SetCommandList(cmd_list);
			Renderer::SetCommandListExecutor(cmd_list_exec);

			scene_hierarchy->Update();
			systems_orchestrator->Update();

			cmd_list_exec->SubmitCommandList(cmd_list);

			cmd_list_exec->SignalAndWait();

			render_context->Present();

		}
	}
}
