#pragma once
#include "Assets/AssetManagement.h"
#include "Graphics/__graphics_api_context.h"

namespace Jigsaw {
	namespace Pipeline {
		/// <summary>
		/// Builder class for compiling RootSignatures. Unlike some other builders, the order in which some of these calls are made is important.
		/// The 'SetNextShaderVisibility' call will have an impact on subsequent 'Push' calls. 
		/// </summary>
		class RootSignatureBuilder {
		public:
			RootSignatureBuilder();
			~RootSignatureBuilder();

			/// <summary>
			/// Indicates the visibility of the subsequent 'Push' calls to the root signature description. 
			/// 
			/// Any encountered Shader visibilities will automatically adjust the Root Signature flags to enable access to the root signature for the corresponding shader type
			/// </summary>
			/// <param name="visibility"></param>
			/// <returns></returns>
			RootSignatureBuilder& SetNextShaderVisiblity(const D3D12_SHADER_VISIBILITY visibility);

			/// <summary>
			/// Inclusive bit-wise or of the current Root Signature flags with the 'flags' parameter
			/// </summary>
			/// <param name="flags"></param>
			/// <returns></returns>
			RootSignatureBuilder& OrRootFlags(D3D12_ROOT_SIGNATURE_FLAGS flags);

			/// <summary>
			/// Sets the current flag value to that of the bit-wise and of the "flags" parameter and the builders "flags" member
			/// </summary>
			/// <param name="flags"></param>
			/// <returns></returns>
			RootSignatureBuilder& AndRootFlags(D3D12_ROOT_SIGNATURE_FLAGS flags);

			/// <summary>
			/// Registers the size of the given template parameter as the next value to be interpreted in the root signature as a series of 32-bit constants
			/// The visibility of the given parameter will be determined by the most recent call to 'SetNextShaderVisibility'
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="register_number"></param>
			/// <returns></returns>
			template <typename T>
			RootSignatureBuilder& PushStructAs32BitConstant(const UINT register_number) {
				size_t obj_size = sizeof(T);
				size_t no_32_bit_constants = obj_size / 4;

				CD3DX12_ROOT_PARAMETER1 root_param;
				root_param.InitAsConstants(no_32_bit_constants, register_number, 0, visibility);
				root_params.push_back(root_param);

				return *this;
			}

			/// <summary>
			/// Completes the construction and builds a RootSignatureResource for use in the pipeline
			/// </summary>
			/// <param name="cmd_list"></param>
			/// <returns></returns>
			Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> Build(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list);

		private:

			std::vector<CD3DX12_ROOT_PARAMETER1> root_params;

			D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			// mutable instance data
			D3D12_SHADER_VISIBILITY visibility;
		};
	}
}
