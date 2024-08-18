#pragma once

#include "../common.hpp"
#include "rain.hlsli"

namespace fb::demos::rain {

inline constexpr std::string_view NAME = "Rain"sv;
inline constexpr ColorAttachmentDescs COLOR_ATTACHMENTS = {ColorAttachmentDesc {
    .format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .clear_color = {0.025f, 0.025f, 0.025f, 1.0f},
}};
inline constexpr DepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 1;

inline constexpr uint PARTICLE_COUNT = 4 * 1024;
inline constexpr uint DISPATCH_COUNT = PARTICLE_COUNT / DISPATCH_X;

struct Parameters {
    float speed = 0.5f;
    float camera_distance = 1.25f;
    float camera_longitude = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(-15.0f);
    float camera_fov = rad_from_deg(60.0f);
    float camera_rotation_speed = 0.05f;
    float particle_width = 0.01f;
    float particle_height = 0.075f;
};

struct Demo {
    Parameters parameters;
    RenderTarget render_target;
    RenderTargetView render_target_view;
    DebugDraw debug_draw;
    GpuBufferDeviceSrvUav<Particle> particles;
    Multibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    GpuPipeline compute_pipeline;
    GpuPipeline draw_pipeline;
    GpuBufferDeviceSrv<Vertex> draw_vertices;
    GpuBufferDeviceIndex<uint16_t> draw_indices;
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

} // namespace fb::demos::rain
