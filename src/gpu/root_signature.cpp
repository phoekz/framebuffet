#include "root_signature.hpp"
#include "descriptors.hpp"

namespace fb {

GpuRootSignature::GpuRootSignature(GpuDevice& device, std::string_view name) {
    // Allocate constants for binding slots.
    CD3DX12_ROOT_PARAMETER1 root_parameter = {};
    root_parameter.InitAsConstants(GpuBindings::CAPACITY, 0, 0);

    // Create the root signature description.
    // Todo: Remove the input assembler.
    auto flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    decltype(desc)::Init_1_2(desc, 1, &root_parameter, 0, nullptr, flags);

    // Serialize the root signature.
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    auto hr = D3DX12SerializeVersionedRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1_2,
        &signature,
        &error);
    if (FAILED(hr)) {
        if (error) {
            FB_LOG_ERROR("{}", (char*)error->GetBufferPointer());
        }
        FB_ASSERT_HR(hr);
    }

    // Create the root signature.
    _root_signature = device.create_root_signature(signature, dx_name(name, "Root Signature"));
}

}  // namespace fb
