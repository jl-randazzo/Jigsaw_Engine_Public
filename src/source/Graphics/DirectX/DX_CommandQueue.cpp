#include "Graphics/DirectX/DX_CommandQueue.h"
#include "Ref.h"
#include "Debug/j_debug.h"
#include "DX_CommandList.h"
#include <string>

#define PRINT_HRESULT_ERRORS_THROW(prestring, hresult) \
	if(FAILED(hresult)) { \
		std::string output = prestring + std::string(_com_error(hresult).ErrorMessage());\
		J_LOG_ERROR(Jigsaw::Graphics::DX_CommandQueue, output.c_str());\
		throw hresult; \
	} 

using namespace Jigsaw::System;
using namespace Microsoft::WRL;

namespace Jigsaw {
	namespace Graphics {
		// CommandQueue implementations
		DX_CommandQueue::DX_CommandQueue(const Microsoft::WRL::ComPtr<ID3D12CommandQueue> dx_cmd_queue, UINT active_allocator_count, UINT buffering_count)
			: dx_cmd_queue(dx_cmd_queue), fence_event(CreateEvent(0, FALSE, FALSE, "FENCE_EVENT")) {
			ComPtr<ID3D12Device> dx_device;
			dx_cmd_queue->GetDevice(__uuidof(ID3D12Device), &dx_device);

			D3D12_COMMAND_QUEUE_DESC desc = dx_cmd_queue->GetDesc();
			D3D12_COMMAND_LIST_TYPE list_type = desc.Type;
			UINT node_mask = desc.NodeMask;

			// creating the command allocators and lists
			for (int i = 0; i < active_allocator_count * buffering_count; i++) {
				ComPtr<ID3D12CommandAllocator> cmd_allocator;
				HRESULT h_cmd_allocator = dx_device->CreateCommandAllocator(list_type, IID_PPV_ARGS(&cmd_allocator));
				PRINT_HRESULT_ERRORS_THROW("Failed to create command allocator: ", h_cmd_allocator);
				dx_command_allocators.push(cmd_allocator);

				ComPtr<ID3D12GraphicsCommandList> cmd_list;
				HRESULT h_cmd_list = dx_device->CreateCommandList(node_mask, list_type, cmd_allocator.Get(), nullptr, IID_PPV_ARGS(&cmd_list));
				PRINT_HRESULT_ERRORS_THROW("Failed to create command list: ", h_cmd_list);
				dx_command_lists.push(cmd_list);
				cmd_list->Close();
			}

			HRESULT fence_creation_result = dx_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx_fence));
			PRINT_HRESULT_ERRORS_THROW("Failed to create fence: ", fence_creation_result);
		}

		ID3D12CommandQueue* DX_CommandQueue::Get() {
			return this->dx_cmd_queue.Get();
		}

		Ref<Jigsaw::Graphics::CommandList> DX_CommandQueue::GetCommandList() {
			{
				std::scoped_lock(queue_mutex);
				// critical read of queue
				if (!dx_command_allocators.empty()) {
					auto cmd_alloc = dx_command_allocators.front();
					dx_command_allocators.pop();

					auto cmd_list = dx_command_lists.front();
					dx_command_lists.pop();

					cmd_alloc->Reset();
					cmd_list->Reset(cmd_alloc.Get(), nullptr);
					cmd_list->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), cmd_alloc.Get());

					// A new Ref to a command list
					return Jigsaw::Ref<Jigsaw::Graphics::CommandList>(new DX_CommandList(cmd_list));
				}
			}

			J_LOG_WARN(DX_CommandQueue, "Command allocator list empty! CPU experiencing starvation");
			std::this_thread::sleep_for(std::chrono::microseconds(3));
		}

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> DX_CommandQueue::GetD3D12CommandQueue() {
			return dx_cmd_queue;
		}

		void DX_CommandQueue::SubmitCommandLists(std::vector<Ref<Jigsaw::Graphics::CommandList>>& jgsw_cmd_lists) {
#ifdef _RENDER_LOG_
			J_LOG_TRACE(DX_CommandQueue, "Submitting {0:d} command list(s) for execution on queue", jgsw_cmd_lists.size())
#endif

			Unique<ID3D12CommandList* []> dx_cmd_lists = Jigsaw::Unique<ID3D12CommandList* []>(new ID3D12CommandList * [jgsw_cmd_lists.size()]);
			Unique<Ref<DX_CommandList>[]> _internals = Unique<Ref<DX_CommandList>[]>(new Ref<DX_CommandList>[jgsw_cmd_lists.size()]);
			for (int i = 0; i < jgsw_cmd_lists.size(); i++) {
				_internals[i] = std::dynamic_pointer_cast<DX_CommandList>(jgsw_cmd_lists[i]);
				J_D_ASSERT_LOG_ERROR(((bool)_internals[i]), DX_CommandQueue, "Error while casting cmd list to DX_CommandList in 'SubmitCommandLists'");
				ComPtr<ID3D12GraphicsCommandList> dx_cmd_list = _internals[i]->GetDXCMDList();
				dx_cmd_lists.get()[i] = dx_cmd_list.Get();
				dx_cmd_list->Close();
			}

			dx_cmd_queue->ExecuteCommandLists(jgsw_cmd_lists.size(), dx_cmd_lists.get());

			for (int i = 0; i < jgsw_cmd_lists.size(); i++) {
				ID3D12CommandAllocator* ptr;
				UINT size = sizeof(ptr);
				dx_cmd_lists.get()[i]->GetPrivateData(__uuidof(ID3D12CommandAllocator), &size, &ptr);

				ComPtr<ID3D12CommandAllocator> cmd_alloc;
				cmd_alloc.Attach(ptr);

				// critical section
				std::scoped_lock<std::mutex>(this->queue_mutex);
				Jigsaw::Ref<DX_CommandList> _internal = std::dynamic_pointer_cast<DX_CommandList>(jgsw_cmd_lists[i]);
				J_D_ASSERT_LOG_ERROR(((bool)_internal), DX_CommandQueue, "Error while casting cmd list to DX_CommandList in 'SubmitCommandLists'");
				dx_command_lists.push(_internal->GetDXCMDList());
				dx_command_allocators.push(cmd_alloc);
			}
		}

		void DX_CommandQueue::SubmitCommandList(Jigsaw::Ref<Jigsaw::Graphics::CommandList>& jgsw_cmd_list) {
#ifdef _RENDER_LOG_
			J_LOG_TRACE(DX_CommandQueue, "Submitting single command list for execution on queue");
#endif

			Jigsaw::Ref<DX_CommandList> _internal = std::dynamic_pointer_cast<DX_CommandList>(jgsw_cmd_list);

			J_D_ASSERT_LOG_ERROR(((bool)_internal), DX_CommandQueue, "Error while casting cmd list to DX_CommandList");
			ComPtr<ID3D12GraphicsCommandList> dx_cmd_list = _internal->GetDXCMDList();

			ID3D12CommandList* const exec_lists[]{
				dx_cmd_list.Get()
			};

			dx_cmd_list->Close();
			dx_cmd_queue->ExecuteCommandLists(1, exec_lists);

			ID3D12CommandAllocator* ptr;
			UINT size = sizeof(ptr);
			dx_cmd_list->GetPrivateData(__uuidof(ID3D12CommandAllocator), &size, &ptr);

			ComPtr<ID3D12CommandAllocator> cmd_alloc;
			cmd_alloc.Attach(ptr);

			// critical section
			std::scoped_lock<std::mutex>(this->queue_mutex);
			dx_command_lists.push(dx_cmd_list);
			dx_command_allocators.push(cmd_alloc);
		}

		void DX_CommandQueue::SignalAndWait() {
			dx_fence->SetEventOnCompletion(fence_value, fence_event);
			dx_cmd_queue->Signal(dx_fence.Get(), fence_value++);

			WaitForSingleObject(fence_event, INFINITE);
		}
	}
}