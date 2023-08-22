#pragma once

#include "../demos.hpp"
#include "cube.hlsli"

namespace fb::demos::cube {

struct Parameters {
    float camera_distance = 5.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.1f;
    float light_latitude = rad_from_deg(60.0f);
    float light_longitude = rad_from_deg(0.0f);
    float light_rotation_speed = 0.5f;
    float light_ambient = 0.25f;
    OutputMode output_mode = OutputMode::Shaded;
};

class CubeDemo {
public:
    static constexpr std::string_view NAME = "Cube"sv;
    static constexpr Float4 CLEAR_COLOR = {0.1f, 0.1f, 0.4f, 1.0f};

    CubeDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;
    auto rt() const -> const RenderTargets& { return _render_targets; }

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
    GpuBufferHostSrv<baked::Vertex> _vertices;
    GpuBufferHostIndex<baked::Index> _indices;
    GpuTextureSrv _base_color_texture;
    GpuTextureSrv _normal_texture;
    GpuTextureSrv _metallic_roughness_texture;
};

} // namespace fb::demos::cube
