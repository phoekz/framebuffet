#pragma once

#include "../gpu/gpu.hpp"

namespace fb {

//
// Constants.
//

inline constexpr uint MAX_ATTACHMENT_COUNT = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;

//
// RenderTargetView.
//

struct KcnColorAttachment {
    GpuTextureRtv* ms_color = nullptr;
    GpuTextureSrvUavRtv* color = nullptr;
    float4 clear_color = {};
};

using KcnColorAttachments = std::array<KcnColorAttachment, MAX_ATTACHMENT_COUNT>;

struct KcnDepthStencilAttachment {
    GpuTextureDsv* depth_stencil = nullptr;
    float clear_depth = 1.0f;
    uint8_t clear_stencil = 0;
};

struct KcnRenderTargetViewDesc {
    uint2 size = {};
    uint sample_count = 1;
    KcnColorAttachments colors = {};
    KcnDepthStencilAttachment depth_stencil = {};
};

class KcnRenderTargetView {
    FB_NO_COPY_MOVE(KcnRenderTargetView);

public:
    KcnRenderTargetView() = default;

    auto create(const KcnRenderTargetViewDesc& desc) -> void;

    auto transition_to_render_target(GpuCommandList& cmd) -> void;
    auto clear(GpuCommandList& cmd) -> void;
    auto transition_to_resolve(GpuCommandList& cmd) -> void;
    auto resolve(GpuCommandList& cmd) -> void;
    auto transition_to_shader_resource(GpuCommandList& cmd) -> void;
    auto set_graphics(GpuGraphicsCommandList& cmd) -> void;

    auto ms_color(uint index) const -> const GpuTextureRtv* { return _desc.colors[index].ms_color; }
    auto ms_color(uint index) -> GpuTextureRtv* { return _desc.colors[index].ms_color; }
    auto color(uint index) const -> const GpuTextureSrvUavRtv* { return _desc.colors[index].color; }
    auto color(uint index) -> GpuTextureSrvUavRtv* { return _desc.colors[index].color; }
    auto depth_stencil() const -> const GpuTextureDsv* { return _desc.depth_stencil.depth_stencil; }
    auto depth_stencil() -> GpuTextureDsv* { return _desc.depth_stencil.depth_stencil; }

    auto color_format(uint index) const -> DXGI_FORMAT {
        return _desc.colors[index].color->format();
    }
    auto depth_format() const -> DXGI_FORMAT { return _desc.depth_stencil.depth_stencil->format(); }
    auto sample_desc() const -> DXGI_SAMPLE_DESC { return {.Count = _desc.sample_count}; }

private:
    KcnRenderTargetViewDesc _desc = {};
};

//
// RenderTarget.
//

struct KcnColorAttachmentDesc {
    DXGI_FORMAT format = {};
    float4 clear_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
};

using KcnColorAttachmentDescs = std::array<KcnColorAttachmentDesc, MAX_ATTACHMENT_COUNT>;

struct KcnDepthStencilAttachmentDesc {
    DXGI_FORMAT format = {};
    float clear_depth = 1.0f;
    uint8_t clear_stencil = 0;
};

struct KcnRenderTargetDesc {
    uint2 size = {};
    uint sample_count = 1;
    KcnColorAttachmentDescs colors = {};
    KcnDepthStencilAttachmentDesc depth_stencil = {};
};

class KcnRenderTarget {
    FB_NO_COPY_MOVE(KcnRenderTarget);

public:
    KcnRenderTarget() = default;

    auto create(GpuDevice& device, const KcnRenderTargetDesc& desc) -> void;

    auto ms_color(uint index) const -> const GpuTextureRtv& { return _ms_colors[index]; }
    auto ms_color(uint index) -> GpuTextureRtv& { return _ms_colors[index]; }
    auto color(uint index) const -> const GpuTextureSrvUavRtv& { return _colors[index]; }
    auto color(uint index) -> GpuTextureSrvUavRtv& { return _colors[index]; }
    auto depth_stencil() const -> const GpuTextureDsv& { return _depth_stencil; }
    auto depth_stencil() -> GpuTextureDsv& { return _depth_stencil; }

    auto color_format(uint index) const -> DXGI_FORMAT { return _desc.colors[index].format; }
    auto depth_format() const -> DXGI_FORMAT { return _desc.depth_stencil.format; }
    auto sample_desc() const -> DXGI_SAMPLE_DESC { return {.Count = _desc.sample_count}; }

    auto view_desc() -> KcnRenderTargetViewDesc;

private:
    KcnRenderTargetDesc _desc = {};
    std::array<GpuTextureRtv, MAX_ATTACHMENT_COUNT> _ms_colors = {};
    std::array<GpuTextureSrvUavRtv, MAX_ATTACHMENT_COUNT> _colors = {};
    GpuTextureDsv _depth_stencil = {};
};

} // namespace fb
