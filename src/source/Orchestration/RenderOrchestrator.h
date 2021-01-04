#ifndef _RENDER_ORCHESTRATOR_H_
#define _RENDER_ORCHESTRATOR_H_

#include "Graphics/RenderContext.h"
#include "Ref.h"

namespace Jigsaw {
	namespace Orchestration {
		class RenderOrchestrator {
		public:
			RenderOrchestrator(const Jigsaw::Ref<Jigsaw::Graphics::RenderContext>& render_context) : render_context(render_context) {}

			void RenderLoop();

		private:
			Jigsaw::Ref<Jigsaw::Graphics::RenderContext> render_context;

		};
	}
}

#endif