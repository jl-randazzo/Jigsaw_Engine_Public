#pragma once
#include "Graphics/CommandListExecutor.h"
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#include <mutex>
#include <dxgi1_6.h>
#include "Graphics/__graphics_api.h"
#include "Graphics/CommandListExecutor.h"
#include "Graphics/CommandList.h"
#include "Graphics/DirectX/DX_CommandQueue.h"
#include "Graphics/RenderContext.h"

#define SWAP_CHAIN_COUNT 2

#define FRAME_NODE 0b001
#define LOAD_NODE 0b010
#define RT_NODE 0b100

#define ACTIVE_FRAME_ALLOCATORS 15 
#define ACTIVE_LOAD_ALLOCATORS 4

namespace Jigsaw {
	namespace Graphics {
		/// <summary>
		/// The DX_Context is one of the primary services exposed to the MainApplicationOrchestrator. It handles the creation of the device for interfacing
		/// with the GPU, can alter RenderTargets in response to changes in the ViewportWindow, and manages the creation, allocation, and submission of ID3D12GraphicsCommandLists.
		/// 
		/// DX_Context is also the final stop point for synchronization with the GPU.
		/// </summary>
		/// <see>
		/// Jigsaw::Graphics::RenderContext
		/// </see>
		/// <author>
		/// Luke Randazzo
		/// </author>
		class DX_Context : public Jigsaw::Graphics::RenderContext {
		public:
			/// <summary>
			/// The public, static function GetDevice provides easy access to certain device functions such as memory allocation.
			/// </summary>
			/// <returns></returns>
			static Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice();

			/// <summary>
			/// PrintDebugMessages is a simple, static debugging tool that logs the queue of debug messages
			/// </summary>
			static void PrintDebugMessages();

			/// <summary>
			/// An instance of DX_Context is required to call the CreateWindowSwapchain function. In response to resizing events or upon creation of the program,
			/// there is a need to define the swapchain and the depth stencil backbuffers
			/// </summary>
			/// <param name="hWnd"></param>
			void CreateWindowSwapchain(const HWND hWnd) override;

			/// <summary>
			/// An instance of DX_Context is required to call the GetCommandList function. The passed in 'type' indicates to the DX_Context if we're looking for
			/// a command list that's optimized for loading or direct execution of draw commands.
			/// </summary>
			/// <param name="type"></param>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::Graphics::CommandList> GetCommandList(Jigsaw::Graphics::JGSW_COMMAND_LIST_TYPE type) override;

			/// <summary>
			/// Returns a DX_CommandQueue object which handles command list submission, execution, and synchronization. 
			/// </summary>
			/// <param name="type"></param>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::Graphics::CommandListExecutor> GetCommandListExecutor(JGSW_COMMAND_LIST_TYPE type) override;

			/// <summary>
			/// Starts the current frame and retrieves a command list that is ready to take pipeline instructions
			/// </summary>
			/// <returns></returns>
			void StartFrame() override;

			/// <summary>
			/// Sets the view information on the cmd_list corresponding to the current frame in the swap chain
			/// </summary>
			/// <param name="cmd_list"></param>
			void SetViews(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd_list) const;

			/// <summary>
			/// Does the underlying transition operations needed to present the current frame
			/// </summary>
			void Present() override;

		private:
			friend Jigsaw::Ref<Jigsaw::Graphics::RenderContext> Jigsaw::Graphics::RenderContext::Init() throw(HRESULT);

			static DX_Context* instance;

			/// <summary>
			/// This method handles the initial construction of the DX_Context and the corresponding Device.
			/// It is intended to be called only by the RenderContext after the target architecture and graphics API (DX12) has been selected
			/// </summary>
			/// <returns></returns>
			static DX_Context* Init() throw(HRESULT);


			/// <summary>
			/// Internal constructor. All of these parameters are defined statically in the Init function
			/// </summary>
			/// <param name="dx_device"></param>
			/// <param name="cmd_queues"></param>
			/// <param name="dx_factory"></param>
			DX_Context(const Microsoft::WRL::ComPtr<ID3D12Device2> dx_device, Jigsaw::Ref<Jigsaw::Graphics::DX_CommandQueue>* const cmd_queues,
				const Microsoft::WRL::ComPtr<IDXGIFactory4> dx_factory) : dx_device(dx_device), cmd_queues(cmd_queues), dx_factory(dx_factory) {}

			/// <summary>
			/// Used internally to create descriptor heaps for a view of a particular 'heap_type'
			/// </summary>
			/// <param name="heap_type"></param>
			/// <param name="count"></param>
			/// <param name="destination"></param>
			void CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, int count, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& destination);

			/// <summary>
			/// Used internally to create a new render target view after the swap_chain was created for the current details of the view window
			/// </summary>
			/// <param name="swap_chain"></param>
			void CreateSwapChainDescriptorsAndRTVs(const Microsoft::WRL::ComPtr<IDXGISwapChain4>& swap_chain);

			/// <summary>
			/// Used internally to create new Depth-Stencil buffers and descriptors in response to changes in the Render Target's dimensions
			/// </summary>
			/// <param name="rtv_backbuffer"></param>
			void CreateDSBuffersAndViews(ID3D12Resource* rtv_backbuffer);

			/// <summary>
			/// Outs a D3D12_VIEWPORT describing the details of the rtv back buffer
			/// </summary>
			/// <param name="viewport"></param>
			/// <param name="back_buffer"></param>
			/// <returns></returns>
			HRESULT CreateViewport(D3D12_VIEWPORT& viewport, const Microsoft::WRL::ComPtr<ID3D12Resource>& back_buffer) const;

			// immutable instance variables
			const Microsoft::WRL::ComPtr<ID3D12Device2> dx_device;
			const Microsoft::WRL::ComPtr<IDXGIFactory4> dx_factory;

			// mutable instance variables
			Jigsaw::Ref<Jigsaw::Graphics::DX_CommandQueue>* const cmd_queues;
			D3D12_VIEWPORT viewport;

			Microsoft::WRL::ComPtr<IDXGISwapChain4> dx_swap_chain;
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dx_swap_descriptor_heap;
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dx_dsv_descriptor_heap;
			Microsoft::WRL::ComPtr<ID3D12Resource>* dx_rtv_backbuffers;
			Microsoft::WRL::ComPtr<ID3D12Resource>* dx_dsv_backbuffers;

			UINT f_i = 0;

		};
	}
}
