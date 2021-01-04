#include "RenderContext.h"
#include "__graphics_api_context.h"

namespace Jigsaw {
	namespace Graphics {

		Jigsaw::Ref<RenderContext> Jigsaw::Graphics::RenderContext::Init() throw(HRESULT) {
			static Jigsaw::Ref<RenderContext> context;

			if (!context) {
#ifdef _J_WIN_10_
			context = Jigsaw::Ref<RenderContext>(DX_Context::Init());
#endif
			}

			return context;
		}

	}
}
