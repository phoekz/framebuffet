#include "samplers.hpp"
#include "device.hpp"

namespace fb {

auto GpuSamplers::create(GpuDevice& device, GpuDescriptors& descriptors) -> void {
    using Sampler = std::tuple<GpuSampler, D3D12_SAMPLER_DESC2, GpuDescriptor>;
    const auto samplers = std::to_array<Sampler>({
        {
            GpuSampler::LinearClamp,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::LinearWrap,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::AnisotropicLinearClamp,
            {
                .Filter = D3D12_FILTER_ANISOTROPIC,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 16,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::AnisotropicLinearWrap,
            {
                .Filter = D3D12_FILTER_ANISOTROPIC,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 16,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::PointClamp,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::PointWrap,
            {
                .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .FloatBorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
        {
            GpuSampler::Shadow,
            {
                .Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                .MipLODBias = 0.0f,
                .MaxAnisotropy = 0,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_LESS,
                .FloatBorderColor = {1.0f, 1.0f, 1.0f, 1.0f},
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .Flags = D3D12_SAMPLER_FLAG_NONE,
            },
            descriptors.sampler().alloc(),
        },
    });

    for (const auto& [type, desc, handle] : samplers) {
        device.create_sampler(desc, handle.cpu());
        _handles[(size_t)type] = handle;
    }
}

} // namespace fb
