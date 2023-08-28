#pragma once

#include "../pch.hpp"

namespace fb {

template<GpuTextureFlags FLAGS>
auto texture_create_and_transfer_baked(
    GpuTexture<FLAGS>& texture,
    GpuDevice& device,
    const baked::Texture& baked,
    D3D12_RESOURCE_STATES before_state,
    D3D12_RESOURCE_STATES after_state,
    std::string_view name
) -> void {
    GpuTextureDesc desc = {
        .format = baked.format,
        .width = baked.width,
        .height = baked.height,
        .mip_count = baked.mip_count,
        .sample_count = 1,
    };

    std::array<GpuTextureTransferDesc, baked::MAX_MIP_COUNT> transfer_descs = {};
    for (uint32_t i = 0; i < baked.mip_count; i++) {
        transfer_descs[i] = GpuTextureTransferDesc {
            .row_pitch = baked.datas[i].row_pitch,
            .slice_pitch = baked.datas[i].slice_pitch,
            .data = baked.datas[i].data.data(),
        };
    }

    texture.create_and_transfer(
        device,
        desc,
        std::span(transfer_descs.data(), baked.mip_count),
        before_state,
        after_state,
        name
    );
}

} // namespace fb
