#ifndef _RENDER_CONTEXT_H_
#define _RENDER_CONTEXT_H_

#include "Graphics/__graphics_api.h"
#include "Ref.h"
#include "CommandList.h"
#include "CommandListExecutor.h"

/// <summary>
/// Links back to the ApplicationRoot's "Run" function
/// </summary>
/// <returns></returns>
long Run();

namespace Jigsaw {
	namespace Graphics {


		/// <summary>
		/// A non-API specific RenderContext that provides virtual handles to all API-related functions. Inheriting classes provide
		/// API-specific implementations of the RenderContext (DX_RenderContext, etc.)
		/// </summary>
		class RenderContext {
		public:

			/// <summary>
			/// An instance of the RenderContext is required to call the CreateWindowSwapchain function. In response to resizing events or upon creation of the Application,
			/// there is a need to define the swapchain and the depth stencil backbuffers.
			/// </summary>
			/// <param name="hWnd"></param>
			virtual void CreateWindowSwapchain(const HWND hWnd) = 0;

			/// <summary>
			/// The RenderContext must independently initialize all that needs to be completed to start the rendering of a frame
			/// </summary>
			virtual void StartFrame() = 0;

			/// <summary>
			/// An instance of RenderContext is required to call the GetCommandList function. The passed in 'type' indicates to the RenderContext if we're looking for
			/// a command list that's optimized for loading or direct execution of draw commands, among other operations.
			/// </summary>
			/// <param name="type"></param>
			/// <returns></returns>
			virtual Jigsaw::Ref<Jigsaw::Graphics::CommandList> GetCommandList(JGSW_COMMAND_LIST_TYPE type) = 0;

			/// <summary>
			/// An instance is required. Returns a CommandListExecutor object that can be used to submit commands of the target 'type.'
			/// </summary>
			/// <param name=""></param>
			/// <returns></returns>
			virtual Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> GetCommandListExecutor(JGSW_COMMAND_LIST_TYPE type) = 0;

			/// <summary>
			/// The final call at the end of any given frame is to 'Present' after all of the CommandLists have been passed to their
			/// Respective CommandListExecutors.
			/// </summary>
			virtual void Present() = 0;

			friend long ::Run(); // the ApplicationRoot's 'Run' function is friended so that the RenderContext can be initialized

			/// <summary>
			///
			/// This method handles the initial construction of the RenderContext and any corresponding Device information.
			/// It is intended to be called only by the ApplicationRoot. In turn, this function is the 'friend' of any child class
			/// that is expected to be statically initialized by the RenderContext.
			///
			/// It is technically a singleton implementation, but the RenderContext and the classes it produces (CommandListExecutor, CommandList)
			/// are passed directly to consuming components/classes as arguments, through constructors, or through dependency injection
			///
			/// </summary>
			/// <returns></returns>
			static Jigsaw::Ref<RenderContext> Init() throw(HRESULT);

		};

	}
}
#endif