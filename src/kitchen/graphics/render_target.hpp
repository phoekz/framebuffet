#pragma once

#include "../gpu/gpu.hpp"

namespace fb::graphics::render_target {

//
// Constants.
//

inline constexpr uint MAX_ATTACHMENT_COUNT = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;

//
// RenderTargetView.
//

struct ColorAttachment {
    GpuTextureRtv* ms_color = nullptr;
    GpuTextureSrvUavRtv* color = nullptr;
    float4 clear_color = {};
};

using ColorAttachments = std::array<ColorAttachment, MAX_ATTACHMENT_COUNT>;

struct DepthStencilAttachment {
    GpuTextureDsv* depth_stencil = nullptr;
    float clear_depth = 1.0f;
    uint8_t clear_stencil = 0;
};

struct RenderTargetViewDesc {
    uint2 size = {};
    uint sample_count = 1;
    ColorAttachments colors = {};
    DepthStencilAttachment depth_stencil = {};
};

class RenderTargetView {
    FB_NO_COPY_MOVE(RenderTargetView);

public:
    RenderTargetView() = default;

    auto create(const RenderTargetViewDesc& desc) -> void;

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
    RenderTargetViewDesc _desc = {};
};

//
// RenderTarget.
//

struct ColorAttachmentDesc {
    DXGI_FORMAT format = {};
    float4 clear_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
};

using ColorAttachmentDescs = std::array<ColorAttachmentDesc, MAX_ATTACHMENT_COUNT>;

struct DepthStencilAttachmentDesc {
    DXGI_FORMAT format = {};
    float clear_depth = 1.0f;
    uint8_t clear_stencil = 0;
};

struct RenderTargetDesc {
    uint2 size = {};
    uint sample_count = 1;
    ColorAttachmentDescs colors = {};
    DepthStencilAttachmentDesc depth_stencil = {};
};

class RenderTarget {
    FB_NO_COPY_MOVE(RenderTarget);

public:
    RenderTarget() = default;

    auto create(GpuDevice& device, const RenderTargetDesc& desc) -> void;

    auto ms_color(uint index) const -> const GpuTextureRtv& { return _ms_colors[index]; }
    auto ms_color(uint index) -> GpuTextureRtv& { return _ms_colors[index]; }
    auto color(uint index) const -> const GpuTextureSrvUavRtv& { return _colors[index]; }
    auto color(uint index) -> GpuTextureSrvUavRtv& { return _colors[index]; }
    auto depth_stencil() const -> const GpuTextureDsv& { return _depth_stencil; }
    auto depth_stencil() -> GpuTextureDsv& { return _depth_stencil; }

    auto color_format(uint index) const -> DXGI_FORMAT { return _desc.colors[index].format; }
    auto depth_format() const -> DXGI_FORMAT { return _desc.depth_stencil.format; }
    auto sample_desc() const -> DXGI_SAMPLE_DESC { return {.Count = _desc.sample_count}; }

    auto view_desc() -> RenderTargetViewDesc;

private:
    RenderTargetDesc _desc = {};
    std::array<GpuTextureRtv, MAX_ATTACHMENT_COUNT> _ms_colors = {};
    std::array<GpuTextureSrvUavRtv, MAX_ATTACHMENT_COUNT> _colors = {};
    GpuTextureDsv _depth_stencil = {};
};

} // namespace fb::graphics::render_target
