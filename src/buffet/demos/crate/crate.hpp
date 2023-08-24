#pragma once

#include "../demos.hpp"
#include "crate.hlsli"

namespace fb::demos::crate {

struct Parameters {
    float camera_distance = 5.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
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
    GpuBufferHostSrv<baked::Vertex> vertices;
    GpuBufferHostIndex<baked::Index> indices;
    GpuTextureSrv base_color;
    GpuTextureSrv normal;
    GpuTextureSrv metallic_roughness;
};

class CrateDemo {
    FB_NO_COPY_MOVE(CrateDemo);

public:
    static constexpr std::string_view NAME = "Crate"sv;
    static constexpr Float4 CLEAR_COLOR = {0.1f, 0.1f, 0.4f, 1.0f};

    CrateDemo() = default;

    auto create(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders)
        -> void;
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;
    auto rt() -> RenderTargets& { return _render_targets; }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    RenderTargets _render_targets;
    DebugDraw _debug_draw;
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    Model _sci_fi_crate;
    Model _metal_plane;
};

} // namespace fb::demos::crate
