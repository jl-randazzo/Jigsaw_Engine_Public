#include "RootSignatureBuilder.h"
#include "Debug/j_debug.h"

#define INIT_SIZE 5

using namespace Microsoft::WRL;
using namespace Jigsaw::Assets;

namespace Jigsaw {
	namespace Pipeline {
		//Public implementations
		RootSignatureBuilder::RootSignatureBuilder() : root_params(), visibility(D3D12_SHADER_VISIBILITY_ALL) {}

		RootSignatureBuilder::~RootSignatureBuilder() {
		}

		Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> RootSignatureBuilder::Build(const Jigsaw::Ref<Jigsaw::Graphics::CommandList>& cmd_list) {
			Jigsaw::Ref<Jigsaw::Graphics::RootSignatureObject> rs_object = Jigsaw::MakeRef<Jigsaw::Graphics::RootSignatureObject>();
#ifdef _J_WIN_10_
			CD3DX12_ROOT_PARAMETER1* p_array = root_params.data();
			UINT size = root_params.size();

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_sig_desc;
			root_sig_desc.Init_1_1(size, p_array, 0, nullptr, flags);

			Microsoft::WRL::ComPtr<ID3DBlob> root_sig_blob;
			Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
			D3DX12SerializeVersionedRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1_1, &root_sig_blob, &error_blob);

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> dx_cmd_list = static_cast<Jigsaw::Graphics::DX_CommandList*>(cmd_list.get())->GetDXCMDList();
			Microsoft::WRL::ComPtr<ID3D12Device> dx_device;
			dx_cmd_list->GetDevice(IID_PPV_ARGS(&dx_device));

			HRESULT rs_creation = dx_device->CreateRootSignature(FRAME_NODE, root_sig_blob->GetBufferPointer(), root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&rs_object->GetRootSignature()));
			J_D_ASSERT_LOG_ERROR(!FAILED(rs_creation), RootSignatureBuilder, "Failed to create Root Signature");
#endif

			return rs_object;
		}

		RootSignatureBuilder& RootSignatureBuilder::SetNextShaderVisiblity(const D3D12_SHADER_VISIBILITY visibility) {
			this->visibility = visibility;

			switch (visibility) {
			case D3D12_SHADER_VISIBILITY_VERTEX:
				AndRootFlags(~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
				break;
			case D3D12_SHADER_VISIBILITY_PIXEL:
				AndRootFlags(~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
				break;
			case D3D12_SHADER_VISIBILITY_GEOMETRY:
				AndRootFlags(~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
				break;
			case D3D12_SHADER_VISIBILITY_HULL:
				AndRootFlags(~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
				break;
			case D3D12_SHADER_VISIBILITY_DOMAIN:
				AndRootFlags(~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
				break;
			}
			return *this;
		}

		RootSignatureBuilder& RootSignatureBuilder::OrRootFlags(D3D12_ROOT_SIGNATURE_FLAGS additional_flags) {
			this->flags |= additional_flags;
			return *this;
		}

		RootSignatureBuilder& RootSignatureBuilder::AndRootFlags(D3D12_ROOT_SIGNATURE_FLAGS additional_flags) {
			this->flags &= additional_flags;
			return *this;
		}
	}
}
