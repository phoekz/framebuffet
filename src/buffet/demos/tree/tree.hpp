#pragma once

#include "../demos.hpp"
#include "tree.hlsli"

namespace fb::demos::tree {

inline constexpr uint32_t SHADOW_MAP_SIZE = 1024;

struct Constants {
    Float4x4 transform;
    Float4x4 light_transform;
    Float3 light_direction;
    float ambient_light = 0.25f;
    float pad[28] = {};
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

  private:
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
