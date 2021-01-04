#ifndef _LOAD_COMMAND_LIST_
#define _LOAD_COMMAND_LIST_

#include "Graphics/__graphics_api.h"
#include "Assets/RuntimeAssets.h"
#include "Graphics/GPURuntimeObjects.h"
#include "Graphics/RENDER_DATA.h"
#include "Ref.h"

namespace Jigsaw {
	namespace Graphics {
		/// <summary>
		/// The enum values are modeled after DirectX12 to support direct casting to proprietary enums. 
		/// </summary>
		enum JGSW_COMMAND_LIST_TYPE {
			JGSW_COMMAND_LIST_TYPE_RENDER = 0,
			JGSW_COMMAND_LIST_TYPE_LOAD = 3
		};

		class CommandList {
		public:

			virtual void DrawIndexed(Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& pl_object, RENDER_DATA& render_data) = 0;

			virtual void LoadBuffer(BYTE* t_arr, size_t t_size, size_t t_count, Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>* buffer_dest) = 0; 

			virtual std::vector<Jigsaw::Graphics::GPUBuffer*>& GetLoadedBuffers();

			virtual bool HasCommands() = 0;

		protected:
			std::vector<Jigsaw::Graphics::GPUBuffer*> loading_buffers;

		};
	}
}
#endif // !_LOAD_COMMAND_LIST_

