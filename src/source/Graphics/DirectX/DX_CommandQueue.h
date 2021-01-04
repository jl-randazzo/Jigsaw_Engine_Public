#ifndef _DX_COMMAND_QUEUE_H_
#define	_DX_COMMAND_QUEUE_H_

#include "Graphics/CommandListExecutor.h"
#include <queue>

namespace Jigsaw {
	namespace Graphics {
		/// <summary>
		/// Private class that encapsulates everything related to DX_CommandQueues, including the management of the lists, allocators, and their fences. 
		/// The idea is that the DX_Context can use the DX_CommandQueue class to manage these things internally and only expose active lists for loading,
		/// rendering, and computing to the MainApplicationOrchestrator. The MainApplicationOrchestrator can then submit lists for execution to the Manager,
		/// and they are sorted based on type. 
		/// </summary>
		class DX_CommandQueue : public Jigsaw::Graphics::CommandListExecutor {
		public:
			DX_CommandQueue(const Microsoft::WRL::ComPtr<ID3D12CommandQueue> dx_cmd_queue, UINT active_allocator_count, UINT buffering_count = 1);

			/// <summary>
			/// Exposes the underlying ID3D12DX_CommandQueue interface to the DX_Context
			/// </summary>
			/// <returns></returns>
			ID3D12CommandQueue* Get();

			/// <summary>
			/// Retrieves a command list for this DX_CommandQueue that has a backing command allocator and is ready to receive instructions
			/// </summary>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::Graphics::CommandList> GetCommandList();

			/// <summary>
			/// Returns the underlying ComPtr to the DirectX CommandQueue object
			/// </summary>
			/// <returns></returns>
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue();

			/// <summary>
			/// Submits a DX_CommandList object to the DX_CommandQueue and executes them
			/// </summary>
			/// <param name="jgsw_cmd_lists"></param>
			virtual void SubmitCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& jgsw_cmd_list) override;

			/// <summary>
			/// Submits a series of DX_CommandList objects to the DX_CommandQueue and executes them
			/// </summary>
			/// <param name="jgsw_cmd_lists"></param>
			virtual void SubmitCommandLists(std::vector<Jigsaw::Ref<Jigsaw::Graphics::CommandList>>& jgsw_cmd_lists) override;

			/// <summary>
			/// Issues a signal and waits on the fence event on this command queue for the execution of the most recently-submitted command list
			/// </summary>
			void SignalAndWait();

			// copy constructor
			DX_CommandQueue& operator=(const DX_CommandQueue& other) {
				dx_command_allocators = other.dx_command_allocators;
				dx_command_lists = other.dx_command_lists;
				dx_cmd_queue = other.dx_cmd_queue;
				fence_event = other.fence_event;
				dx_fence = other.dx_fence;

				return *this;
			}

		private:

			std::mutex queue_mutex;
			UINT fence_value = 0;

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> dx_cmd_queue;
			Microsoft::WRL::ComPtr<ID3D12Fence> dx_fence;
			std::queue<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> dx_command_allocators;
			std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> dx_command_lists;
			HANDLE fence_event;

			// Inherited via LoadCommandListExecutor
		};
	}
}

#endif // !_DX_COMMAND_QUEUE_H_
