#pragma once

#include "demo.hpp"

namespace fb::anim {

struct Constants {
    Matrix transform;
    float pad[48] = {};
};

class Demo {
  public:
    static constexpr std::string_view NAME = "Anim"sv;
    static constexpr Vector4 CLEAR_COLOR = {0.6f, 0.2f, 0.6f, 1.0f};

    Demo(GpuDevice& device);
    auto gui(const gui::Desc& desc) -> void;
    auto update(const demo::UpdateDesc& desc) -> void;
    auto render(GpuDevice& device, GpuCommandList& cmd) -> void;

    auto rt_color() const -> const GpuTexture2dSrvRtv& { return _render_targets.color(); }

  private:
    GpuRenderTargets _render_targets;
    GpuDebugDraw _debug_draw;
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constant_buffer;
    GpuBufferHostSrv<GltfSkinningVertex> _vertex_buffer;
    GpuBufferHostIndex<GltfIndex> _index_buffer;
    GpuBufferHostSrv<Matrix> _joint_inverse_bind_buffer;
    GpuBufferHostSrv<Matrix> _joint_global_transform_buffer;
    GpuTexture2dSrv _texture;
    float _animation_time = 0.0f;
    float _animation_duration = 0.0f;
    GltfModel _model;
    std::vector<Matrix> _node_global_transforms;
};

}  // namespace fb::anim
