#pragma once

#include "../common.hpp"
#include "crate.hlsli"

namespace fb::demos::crate {

inline constexpr std::string_view NAME = "Crate"sv;
inline constexpr KcnColorAttachmentDescs COLOR_ATTACHMENTS = {KcnColorAttachmentDesc {
    .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
    .clear_color = {0.0f, 0.0f, 0.0f, 1.0f},
}};
inline constexpr KcnDepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
};
inline constexpr uint SAMPLE_COUNT = 4;

struct Parameters {
    float camera_distance = 3.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(35.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.1f;
    float light_latitude = rad_from_deg(35.0f);
    float light_longitude = rad_from_deg(0.0f);
    float light_rotation_speed = 1.0f;
    float light_ambient = 0.25f;
    float light_intensity = 2.0f;
    OutputMode output_mode = OutputMode::Shaded;
};

struct Model {
    GpuBufferDeviceSrv<baked::Vertex> vertices;
    GpuBufferDeviceIndex<baked::Index> indices;
    GpuTextureSrv base_color;
    GpuTextureSrv normal;
    GpuTextureSrv metallic_roughness;
};

struct Demo {
    Parameters parameters;
    KcnRenderTarget render_target;
    KcnRenderTargetView render_target_view;
    KcnDebugDraw debug_draw;
    GpuPipeline pipeline;
    KcnMultibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
    Model sci_fi_crate;
    Model metal_plane;
    GpuTextureSrv pbr_lut;
    GpuTextureSrvCube pbr_irr;
    GpuTextureSrvCube pbr_rad;
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

} // namespace fb::demos::crate
