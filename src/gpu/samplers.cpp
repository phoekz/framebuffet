#include "samplers.hpp"

namespace fb {

GpuSamplers::GpuSamplers(GpuDevice& device, GpuDescriptors& descriptors) {
    std::tuple<GpuSamplerType, D3D12_SAMPLER_DESC, GpuDescriptorHandle> samplers[] = {
        {
            GpuSamplerType::LinearClamp,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSamplerType::LinearWrap,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSamplerType::Shadow,
            {
                .Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
                .BorderColor = {1.0f, 1.0f, 1.0f, 1.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
            },
            descriptors.sampler().alloc(),
        },
    };

    for (const auto& [type, desc, handle] : samplers) {
        device.create_sampler(desc, handle.cpu());
        _handles[(size_t)type] = handle;
    }
}

}  // namespace fb
