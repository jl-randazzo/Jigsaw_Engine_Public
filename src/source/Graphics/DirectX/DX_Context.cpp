#include "DX_Context.h"
#include "DX_CommandList.h"
#include "Debug/j_debug.h"
#include "Graphics/DirectX/DX_CommandQueue.h"
#include "xd3d12.h"

const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

using namespace Microsoft::WRL;
using namespace Jigsaw::System;

#define PRINT_HRESULT_ERRORS_THROW(prestring, hresult) \
	if(FAILED(hresult)) { \
		DX_Context::PrintDebugMessages(); \
		std::string output = prestring + std::string(_com_error(hresult).ErrorMessage());\
		J_LOG_ERROR(Jigsaw::Graphics::DX_Context, output.c_str());\
		throw hresult; \
	} 

namespace Jigsaw {
	namespace Graphics {

		// DX_Context implementations
		/**
		* Constructor
		*/

		DX_Context* DX_Context::instance;

		ComPtr<ID3D12Device2> DX_Context::GetDevice() {
			return instance->dx_device;
		}

		/**
		* Basic utility for printing error/debug messages to the console when something fails
		*/
		void LocalPrintDebugMessages(ComPtr <ID3D12Device2> dx_device) {
			ComPtr<ID3D12InfoQueue> info_queue;
			HRESULT cast_result = dx_device.As(&info_queue);
			PRINT_HRESULT_ERRORS_THROW("Failed to cast device to info queue: ", cast_result);

			UINT count = info_queue->GetNumStoredMessages();
			for (int i = 0; i < count; i++) {
				SIZE_T length;
				HRESULT h = info_queue->GetMessage(i, NULL, &length);

				D3D12_MESSAGE* message = (D3D12_MESSAGE*)malloc(length);
				h = info_queue->GetMessage(i, message, &length);

				J_LOG_INFO(DX_Context, message->pDescription);
				free(message);
			}
		}

		void DX_Context::PrintDebugMessages() {
			if (instance == nullptr) {
				return;
			}
			else {
				LocalPrintDebugMessages(instance->dx_device);
			}
		}

		/**
		* Creates a swap chain with standard settings for the target window and returns a ComPtr to that SwapChain's interface.
		*/
		void DX_Context::CreateWindowSwapchain(const HWND hWnd) {
			DXGI_SWAP_CHAIN_DESC1 swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(DXGI_SWAP_CHAIN_DESC1));
			swap_chain_description.Width = 0; // use the width value of the associated HWND 
			swap_chain_description.Height = 0; // use the height value of the associated HWND 
			swap_chain_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.BufferCount = SWAP_CHAIN_COUNT;
			swap_chain_description.Stereo = FALSE;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Scaling = DXGI_SCALING_STRETCH;
			swap_chain_description.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swap_chain_description.Flags = 0;

			// building the actual swap chain
			ComPtr<IDXGISwapChain1> swap_chain_1;
			HRESULT swap_chain_creation_result = dx_factory->CreateSwapChainForHwnd(cmd_queues[D3D12_COMMAND_LIST_TYPE_DIRECT]->GetD3D12CommandQueue().Get(), hWnd, &swap_chain_description, nullptr, nullptr, &swap_chain_1);
			PRINT_HRESULT_ERRORS_THROW("Failed to create Swap Chain: ", swap_chain_creation_result);

			HRESULT cast_result = swap_chain_1.As(&dx_swap_chain);

			J_LOG_INFO(DX_Context, "Swap chain created");

			CreateSwapChainDescriptorsAndRTVs(dx_swap_chain);
			CreateDSBuffersAndViews(dx_rtv_backbuffers[0].Get());
			CreateViewport(viewport, dx_rtv_backbuffers[0].Get());
		}

		Jigsaw::Ref<Jigsaw::Graphics::CommandList> DX_Context::GetCommandList(JGSW_COMMAND_LIST_TYPE type) {
			DX_CommandQueue* dx_command_queue = static_cast<DX_CommandQueue*>(cmd_queues[(D3D12_COMMAND_LIST_TYPE)type].get());
			auto cmd_list = dx_command_queue->GetCommandList();
			if (type == JGSW_COMMAND_LIST_TYPE_RENDER) {
				DX_CommandList* dx_cmd_list = static_cast<DX_CommandList*>(cmd_list.get());

				// setting the appropriate views on the given command list
				XD3D12_CPU_DESCRIPTOR_HANDLE rtv_handle(dx_swap_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				XD3D12_CPU_DESCRIPTOR_HANDLE dsv_handle(dx_dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				dx_cmd_list->SetViews(&rtv_handle.get(f_i), &dsv_handle.get(f_i), viewport);
			}

			return cmd_list;
		}

		/**
		* A utility for creating a basic descriptor heap and storing it in the destination
		*/
		void DX_Context::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, int count, ComPtr<ID3D12DescriptorHeap>& destination) {
			D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
			descriptor_heap_desc.Type = heap_type;
			descriptor_heap_desc.NumDescriptors = count;
			descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			descriptor_heap_desc.NodeMask = 0;

			HRESULT result = dx_device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), &destination);
			PRINT_HRESULT_ERRORS_THROW("Failed to create descriptor heap: ", result);
		}

		/**
		* uses the specified swap chain to create a descriptor heap and then initialize the render target views
		*/
		void DX_Context::CreateSwapChainDescriptorsAndRTVs(const ComPtr<IDXGISwapChain4>& swap_chain) {
			CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SWAP_CHAIN_COUNT, dx_swap_descriptor_heap);

			XD3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle(dx_swap_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			dx_rtv_backbuffers = new ComPtr<ID3D12Resource>[SWAP_CHAIN_COUNT];

			// bind the back buffers to the corresponding descriptor handles one by one and create the render target views
			for (int i = 0; i < SWAP_CHAIN_COUNT; i++) {
				swap_chain->GetBuffer(i, IID_PPV_ARGS(&dx_rtv_backbuffers[i]));
				dx_device->CreateRenderTargetView(dx_rtv_backbuffers[i].Get(), nullptr, descriptor_handle);
				descriptor_handle.operator++();
			}

			return;
		}

		void DX_Context::CreateDSBuffersAndViews(ID3D12Resource* rtv_backbuffer) {
			D3D12_CLEAR_VALUE clear_val;
			clear_val.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			clear_val.DepthStencil.Depth = 1.0f;
			clear_val.DepthStencil.Stencil = 0;

			D3D12_RESOURCE_DESC rtv_buffer_desc = rtv_backbuffer->GetDesc();
			D3D12_RESOURCE_DESC dsv_buffer_desc = rtv_buffer_desc;
			dsv_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsv_buffer_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			// create descriptor heap for Depth Stencil View
			CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, SWAP_CHAIN_COUNT, dx_dsv_descriptor_heap);

			XD3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle(dx_dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			dx_dsv_backbuffers = new ComPtr<ID3D12Resource>[SWAP_CHAIN_COUNT];

			// Create the buffers and map the views to the descriptor heap
			for (int i = 0; i < SWAP_CHAIN_COUNT; i++) {
				HRESULT result = dx_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
					&dsv_buffer_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_val, IID_PPV_ARGS(&dx_dsv_backbuffers[i]));
				PRINT_HRESULT_ERRORS_THROW("Failed to create depth stencil: ", result);
				dx_device->CreateDepthStencilView(dx_dsv_backbuffers[i].Get(), nullptr, descriptor_handle);
				descriptor_handle.operator++();
			}
		}

		HRESULT DX_Context::CreateViewport(D3D12_VIEWPORT& viewport, const ComPtr <ID3D12Resource>& back_buffer) const {
			D3D12_RESOURCE_DESC back_buffer_desc = back_buffer->GetDesc();

			ZeroMemory(&viewport, sizeof(D3D12_VIEWPORT));
			viewport.Height = back_buffer_desc.Height;
			viewport.Width = back_buffer_desc.Width;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;

			return 0;
		}

		Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> DX_Context::GetCommandListExecutor(JGSW_COMMAND_LIST_TYPE type) {
			return std::dynamic_pointer_cast<CommandListExecutor>(cmd_queues[type]);
		}

		/**
		* The Device, command queue, and target factory are 'const' values and need to be initialized before constructing the actual
		* instance of the DirectX 12 manager. This private, static method handles the initialization of those immutable things so that
		* if the display properties or swap chain need to change, they may, but it will not result in a reconstruction of the DX_Context
		* or the pipeline itself.
		*/
		DX_Context* DX_Context::Init() {
			if (instance != nullptr) // this should only ever be called one time by ApplicationRoot 
				throw - 1;

			ComPtr<ID3D12Device2> device;
			D3D_FEATURE_LEVEL feature_level;

			// creating the initial factory for adapters, etc.
			ComPtr<IDXGIFactory4> factory;
			HRESULT factory_creation_result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
			PRINT_HRESULT_ERRORS_THROW("Factory creation failure: ", factory_creation_result)

				// loading the adapter and casting it to version 4
				ComPtr<IDXGIAdapter> adapter;
			ComPtr<IDXGIAdapter4> adapter_4;

			HRESULT retrieve_adapter = factory->EnumAdapters(0, &adapter);
			PRINT_HRESULT_ERRORS_THROW("Adapter retrieve failure: ", retrieve_adapter)
				HRESULT cast_adapter = adapter.As(&adapter_4);
			PRINT_HRESULT_ERRORS_THROW("Adapter cast failure: ", cast_adapter)

				//enabling debug layer for D3D12 prior to creating the device
				ComPtr<ID3D12Debug> debug;
			D3D12GetDebugInterface(__uuidof(ID3D12Debug), &debug);
			debug->EnableDebugLayer();

			// creating the main device
			HRESULT create_device = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
			PRINT_HRESULT_ERRORS_THROW("Device creation failure code: ", create_device)

				ComPtr<ID3D12CommandQueue> frame_queue, load_queue;

			// creating the frame-render command queue
			D3D12_COMMAND_QUEUE_DESC frame_queue_desc = XD3D12_COMMAND_QUEUE_DESC::FrameQueue(FRAME_NODE);
			HRESULT frame_queue_result = device->CreateCommandQueue(&frame_queue_desc, __uuidof(ID3D12CommandQueue), &frame_queue);
			PRINT_HRESULT_ERRORS_THROW("Command queue creation failure: ", frame_queue_result)

				// creating the loading command queue
				D3D12_COMMAND_QUEUE_DESC load_queue_desc = XD3D12_COMMAND_QUEUE_DESC::LoadQueue(FRAME_NODE);
			HRESULT load_queue_result = device->CreateCommandQueue(&load_queue_desc, __uuidof(ID3D12CommandQueue), &load_queue);
			PRINT_HRESULT_ERRORS_THROW("Command queue creation failure: ", load_queue_result);

			// populating the command queue map
			Ref<Jigsaw::Graphics::DX_CommandQueue>* queue_array = new Ref<Jigsaw::Graphics::DX_CommandQueue>[4];
			queue_array[D3D12_COMMAND_LIST_TYPE_DIRECT] = MakeRef<DX_CommandQueue>(frame_queue, ACTIVE_FRAME_ALLOCATORS);
			queue_array[D3D12_COMMAND_LIST_TYPE_COPY] = MakeRef<DX_CommandQueue>(load_queue, ACTIVE_LOAD_ALLOCATORS);

			J_LOG_INFO(DX_Context, "Device creation successful");
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			instance = new DX_Context(device, queue_array, factory);
			return instance;
		}

		void DX_Context::StartFrame() {
			Jigsaw::Ref<DX_CommandQueue>& render_queue = cmd_queues[D3D12_COMMAND_LIST_TYPE_DIRECT];

			auto cmd_list = render_queue->GetCommandList();
			Jigsaw::Ref<DX_CommandList> dx_cmd_list_ref = std::dynamic_pointer_cast<DX_CommandList>(cmd_list);
			auto dx_cmd_list = dx_cmd_list_ref->GetDXCMDList();

			CD3DX12_RESOURCE_BARRIER rtv_barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx_rtv_backbuffers[f_i].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			dx_cmd_list->ResourceBarrier(1, &rtv_barrier);

			// clearing the main render target and the depth-stencil buffer
			const float bg[]{ .01f, 0.5f, 0.5f, 1.0f };
			XD3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle(dx_swap_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			XD3D12_CPU_DESCRIPTOR_HANDLE dsv_handle(dx_dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			dx_cmd_list->ClearRenderTargetView(descriptor_handle.get(f_i), bg, 0, nullptr);
			dx_cmd_list->ClearDepthStencilView(dsv_handle.get(f_i), D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, nullptr);

			render_queue->SubmitCommandList(cmd_list);
		}

		/**
		* Delivers the currently selected views to the cmd_list
		*/
		void DX_Context::SetViews(ComPtr<ID3D12GraphicsCommandList> cmd_list) const {
			XD3D12_CPU_DESCRIPTOR_HANDLE rtv_handle(dx_swap_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			XD3D12_CPU_DESCRIPTOR_HANDLE dsv_handle(dx_dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), dx_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			cmd_list->OMSetRenderTargets(1, &rtv_handle.get(f_i), FALSE, &dsv_handle.get(f_i));
			cmd_list->RSSetViewports(1, &viewport);
			CD3DX12_RECT scissor(0, 0, LONG_MAX, LONG_MAX);
			cmd_list->RSSetScissorRects(1, &scissor);
		}

		// f_i = frame index
		void DX_Context::Present() {
			// retrieve a final command list to transition back buffer to display state
			DX_CommandQueue& frame_queue = *cmd_queues[D3D12_COMMAND_LIST_TYPE_DIRECT].get();
			Ref<CommandList> list = frame_queue.GetCommandList();
			Ref<DX_CommandList> cmd_list = std::dynamic_pointer_cast<DX_CommandList>(list);

			ComPtr<ID3D12GraphicsCommandList> dx_cmd_list = cmd_list->GetDXCMDList();
			CD3DX12_RESOURCE_BARRIER present_barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx_rtv_backbuffers[f_i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			dx_cmd_list->ResourceBarrier(1, &present_barrier);

			frame_queue.SubmitCommandList(list);

			dx_swap_chain->Present(1, 0);
			frame_queue.SignalAndWait();
			J_LOG_TRACE(DX_Context, "Presented frame")

				f_i += 1;
			f_i = f_i == SWAP_CHAIN_COUNT ? 0 : f_i;
		}
	}
}
