#ifndef _DX_COMMAND_LIST_H_
#define _DX_COMMAND_LIST_H_

#include "Graphics/CommandList.h"

namespace Jigsaw {
	namespace Graphics {
		/// <summary>
		/// DX_CommandList converts the Jigsaw-level API components into DX12 command list submissions which must be subsequently 
		/// passed to a command list executor.
		/// </summary>
		class DX_CommandList : public Jigsaw::Graphics::CommandList {
		public:
			/// <summary>
			/// Internal function to DX_CommandList sets the corresponding views
			/// </summary>
			/// <param name="rtv_handle"></param>
			/// <param name="dsv_handle"></param>
			/// <param name="viewport"></param>
			void SetViews(D3D12_CPU_DESCRIPTOR_HANDLE* rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle, D3D12_VIEWPORT& viewport);

			/// <summary>
			/// Sole constructor. 
			/// </summary>
			/// <param name="dx_cmd_list"></param>
			DX_CommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& dx_cmd_list) : dx_cmd_list(dx_cmd_list) {}

			/// <summary>
			/// Implementation for the Jigsaw CommandList interface's "LoadBuffer" method
			/// </summary>
			/// <param name="t_arr"></param>
			/// <param name="t_size"></param>
			/// <param name="t_count"></param>
			/// <param name="buffer_dest"></param>
			virtual void LoadBuffer(BYTE* t_arr, size_t t_size, size_t t_count, Jigsaw::Ref<Jigsaw::Graphics::GPUBuffer>* buffer_dest) override;

			/// <summary>
			/// files commands for drawing the 'render_data' on the current cmd list
			/// </summary>
			/// <param name="pl_object"></param>
			/// <param name="render_data"></param>
			virtual void DrawIndexed(Jigsaw::Ref<Jigsaw::Graphics::PipelineObject>& pl_object, RENDER_DATA& render_data) override;

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			virtual bool HasCommands() override;

			/// <summary>
			/// Returns a ComPtr to the underlying ID3D12GraphicsCommandList interface, but destroys the local reference to the
			/// dx_cmd_list. This command effectively nullifies the underlying DX_CommandList object
			/// </summary>
			/// <returns></returns>
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetDXCMDList();

			ID3D12GraphicsCommandList* operator->();

		private:

			bool command_list_dirty = false;

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> dx_cmd_list;

		};
	}
}

#endif // !_DX_COMMAND_LIST_H_
