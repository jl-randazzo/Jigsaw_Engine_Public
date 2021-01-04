#ifdef _RUN_UNIT_TESTS_
#include "MockCommandListExecutor.h"

using namespace Jigsaw::Graphics;

void ProjectTests::MockCommandListExecutor::SubmitCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list)
{
	this->list_invocation_count += 1;
}

void ProjectTests::MockCommandListExecutor::SubmitCommandLists(std::vector<Jigsaw::Ref<Jigsaw::Graphics::CommandList>>& cmd_lists) {
	this->list_invocation_count += cmd_lists.size();
}

void ProjectTests::MockCommandListExecutor::SignalAndWait() {
}

void ProjectTests::MockCommandList::DrawIndexed(Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& pl_object, RENDER_DATA& render_data) {
	has_commands = true;
}

void ProjectTests::MockCommandList::LoadBuffer(BYTE* t_arr, size_t t_size, size_t t_count, Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>* buffer_dest) {
	has_commands = true;
}

bool ProjectTests::MockCommandList::HasCommands() {
	bool _has = has_commands;
	has_commands = false;
	return _has;
}

void ProjectTests::MockRenderContext::CreateWindowSwapchain(const HWND hWnd)
{
}

void ProjectTests::MockRenderContext::StartFrame()
{
}

Jigsaw::Ref<Jigsaw::Graphics::CommandList> ProjectTests::MockRenderContext::GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type)
{
	return Jigsaw::Ref<Jigsaw::Graphics::CommandList>(new MockCommandList);
}

Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> ProjectTests::MockRenderContext::GetCommandListExecutor(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type)
{
	return Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor>(new MockCommandListExecutor);
}

void ProjectTests::MockRenderContext::Present()
{
}
#endif
