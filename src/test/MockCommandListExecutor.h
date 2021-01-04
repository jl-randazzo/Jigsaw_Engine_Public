#ifdef _RUN_UNIT_TESTS_
#ifndef _MOCK_COMMAND_LIST_EXECUTOR_H_
#define _MOCK_COMMAND_LIST_EXECUTOR_H_

#include "Graphics/CommandListExecutor.h"
#include "Graphics/RenderContext.h"

namespace ProjectTests {
	class MockCommandList : public Jigsaw::Graphics::CommandList {
	public:

		// Inherited via CommandList
		virtual void DrawIndexed(Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& pl_object, Jigsaw::Graphics::RENDER_DATA& render_data) override;
		virtual void LoadBuffer(BYTE* t_arr, size_t t_size, size_t t_count, Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>* buffer_dest) override;
		virtual bool HasCommands() override;

	private:
		bool has_commands = false;
	};
	class MockCommandListExecutor : public Jigsaw::Graphics::CommandListExecutor {
	public:

		// Inherited via CommandListExecutor
		virtual void SubmitCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list) override;

		virtual void SubmitCommandLists(std::vector<Jigsaw::Ref<Jigsaw::Graphics::CommandList>>& cmd_lists) override;

		virtual void SignalAndWait() override;

		int list_invocation_count = 0;
	};

	class MockRenderContext : public Jigsaw::Graphics::RenderContext {
	public:
		MockRenderContext() {}


		// Inherited via RenderContext
		virtual void CreateWindowSwapchain(const HWND hWnd) override;

		virtual void StartFrame() override;

		virtual Jigsaw::Ref<Jigsaw::Graphics::CommandList> GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type) override;

		virtual Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> GetCommandListExecutor(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type) override;

		virtual void Present() override;

	};
}

#endif // !_MOCK_COMMAND_LIST_EXECUTOR_H_
#endif
