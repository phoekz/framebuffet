#pragma once

#include "../common.hpp"
#include "text.hlsli"

namespace fb::demos::text {

inline constexpr std::string_view NAME = "Text"sv;
inline constexpr KcnColorAttachmentDescs COLOR_ATTACHMENTS = {KcnColorAttachmentDesc {
    .format = DXGI_FORMAT_R16G16B16A16_UNORM,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
}};
inline constexpr KcnDepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 4;

inline constexpr uint MAX_GLYPH_COUNT = 1024;
inline constexpr std::string_view TEXT = "framebuffet!"sv;

struct Parameters {
    float camera_distance = 3.0f;
    float camera_fov = rad_from_deg(70.0f);
    float camera_latitude = rad_from_deg(-5.0f);
    float camera_longitude = rad_from_deg(90.0f);
    float3 text_offset;
    float scene_rotation_angle = 0.0f;
    float scene_rotation_speed = 0.3f;
    float4 glyph_color = float4(0.8f, 0.8f, 0.8f, 1.0f);
};

using DrawGlyphCommand = IndirectDrawIndexedCommand<GlyphBindings>;

struct Demo {
    Parameters parameters;
    KcnRenderTarget render_target;
    KcnRenderTargetView render_target_view;
    KcnDebugDraw debug_draw;

    struct {
        GpuTextureSrvCube irr;
    } pbr;

    struct {
        GpuPipeline pipeline;
        KcnMultibuffer<GpuBufferHostCbv<BackgroundConstants>, FRAME_COUNT> constants;
        GpuBufferDeviceSrv<baked::Vertex> vertices;
        GpuBufferDeviceIndex<baked::Index> indices;
    } bg;

    struct {
        GpuPipeline pipeline;
        KcnMultibuffer<GpuBufferHostCbv<GlyphConstants>, FRAME_COUNT> constants;
        KcnMultibuffer<GpuBufferHostSrv<GlyphInstance>, FRAME_COUNT> instances;
        std::vector<baked::Submesh> submeshes;
        GpuBufferDeviceSrv<baked::Vertex> vertices;
        GpuBufferDeviceIndex<baked::Index> indices;

        ComPtr<ID3D12CommandSignature> indirect_command_signature;
        KcnMultibuffer<GpuBufferHostSrv<DrawGlyphCommand>, FRAME_COUNT> indirect_commands;
        KcnMultibuffer<GpuBufferHostSrv<uint>, FRAME_COUNT> indirect_counts;

        std::vector<baked::Glyph> glyphs;
        uint glyph_submesh_count = 0;
        uint glyph_submeshes[MAX_GLYPH_COUNT] = {};
        char text_buffer[MAX_GLYPH_COUNT] = {};
        char first_character = '\0';
        char last_character = '\0';
        float ascender = 0.0f;
        float space_advance = 0.0f;
    } glyph;
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
};

auto create(Demo& demo, const CreateDesc& desc) -> void;
auto gui(Demo& demo, const GuiDesc& desc) -> void;
auto update(Demo& demo, const UpdateDesc& desc) -> void;
auto render(Demo& demo, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demo& demo, A& arc) -> void {
    arc & demo.parameters;
}

} // namespace fb::demos::text
