#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Graphics/RENDER_DATA.h"
#include "Pipeline/JigsawPipeline.h"
#include "Graphics/CommandList.h"
#include "Graphics/CommandListExecutor.h"

class Renderer {
public:
	static void DrawIndexed(Jigsaw::Pipeline::JIGSAW_PIPELINE pipeline, Jigsaw::Graphics::RENDER_DATA& render_data);
	static void SetCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list);
	static void SetCommandListExecutor(Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor>& cmd_list_executor);

private:
	static Jigsaw::Ref<Jigsaw::Graphics::CommandList> cmd_list;
	static Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> cmd_list_exec;
};
#endif