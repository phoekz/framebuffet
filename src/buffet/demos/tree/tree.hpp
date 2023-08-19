#pragma once

#include "../demos.hpp"
#include "tree.hlsli"

namespace fb::demos::tree {

inline constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct Parameters {
    float ambient_light = 0.25f;
    float light_projection_size = 15.0f;
    float light_longitude = 0.0f;
    float light_latitude = rad_from_deg(30.0f);
    float light_distance = 15.0f;
    float shadow_near_plane = 0.1f;
    float shadow_far_plane = 100.0f;
    float camera_distance = 10.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
};

class TreeDemo {
public:
    static constexpr std::string_view NAME = "Tree"sv;
    static constexpr Float4 CLEAR_COLOR = {0.32549f, 0.51373f, 0.56078f, 1.0f};

    TreeDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
    auto gui(const GuiDesc& desc) -> void;
    auto update(const UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTextureSrvRtv& { return _render_targets.color(); }

    template<Archive A>
    auto archive(A& arc) -> void {
        arc& _parameters;
    }

private:
    Parameters _parameters;
    RenderTargets _render_targets;
    DebugDraw _debug_draw;

    GpuBufferHostCbv<Constants> _constants;

    GpuBufferHostSrv<baked::Vertex> _tree_vertices;
    GpuBufferHostIndex<baked::Index> _tree_indices;
    GpuTextureSrv _tree_texture;

    GpuBufferHostSrv<baked::Vertex> _plane_vertices;
    GpuBufferHostIndex<baked::Index> _plane_indices;
    GpuTextureSrv _plane_texture;

    GpuPipeline _shadow_pipeline;
    GpuTextureSrvDsv _shadow_depth;

    GpuPipeline _draw_pipeline;
};

} // namespace fb::demos::tree
