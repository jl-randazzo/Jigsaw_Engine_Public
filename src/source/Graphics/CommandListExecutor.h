#ifndef _COMMAND_LIST_EXECUTOR_H_
#define _COMMAND_LIST_EXECUTOR_H_

#include <windows.h>
#include <wrl.h>
#include <comdef.h>
#include <d3d12.h>
#include "Ref.h"
#include "Graphics/CommandList.h"

namespace Jigsaw {
	namespace Graphics {

		/// <summary>
		/// The CommandListExecutor is a privileged interface that handles direct interaction with the GPU for execution load/copy
		/// commands or Render commands.
		/// 
		/// Each Graphics API (DirectX 12, etc.) will have its own implementation of a CommandListExecutor
		/// </summary>
		class CommandListExecutor {
		public:

			/// <summary>
			/// Executes a single command list on the Graphics API
			/// After Execution, the passed in CommandList is no longer usable
			/// </summary>
			/// <param name="cmd_list"></param>
			virtual void SubmitCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list) = 0;

			/// <summary>
			/// Executes multiple command lists on the Graphics API. Some APIs may support concurrent execution; some may
			/// execute the included command lists back to back to back.
			///
			/// After Execution, the passed in CommandLists are no longer usable
			/// </summary>
			/// <param name="cmd_lists"></param>
			virtual void SubmitCommandLists(std::vector<Jigsaw::Ref<Jigsaw::Graphics::CommandList>>& cmd_lists) = 0;

			/// <summary>
			/// Awaits the execution of submitted command lists
			/// </summary>
			virtual void SignalAndWait() = 0;

		};
	}
}

#endif // !_COMMAND_LIST_EXECUTOR_H_

