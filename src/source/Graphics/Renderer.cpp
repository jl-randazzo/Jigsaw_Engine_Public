#include "Renderer.h"
#include "Pipeline/PipelineService.h"

Jigsaw::Ref<Jigsaw::Graphics::CommandList> Renderer::cmd_list;
Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> Renderer::cmd_list_exec;

void Renderer::DrawIndexed(Jigsaw::Pipeline::JIGSAW_PIPELINE pipeline, Jigsaw::Graphics::RENDER_DATA& render_data) {
	auto pl = Jigsaw::Pipeline::PipelineServiceManager::GetPipeline(pipeline);
	cmd_list->DrawIndexed(pl, render_data);
}

void Renderer::SetCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& _cmd_list) {
	cmd_list = _cmd_list;
}

void Renderer::SetCommandListExecutor(Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor>& _cmd_list_executor) {
	cmd_list_exec = _cmd_list_executor;
}
