#pragma once

#include "../demos.hpp"
#include "anim.hlsli"

namespace fb::demos::anim {

struct Parameters {
    float camera_distance = 4.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.5f;
    Float2 camera_clip_planes = Float2(0.1f, 300.0f);
};

class AnimDemo {
public:
    static constexpr std::string_view NAME = "Anim"sv;
    static constexpr Float4 CLEAR_COLOR = {0.6f, 0.2f, 0.6f, 1.0f};

    AnimDemo(GpuDevice& device, const baked::Assets& assets, const baked::Shaders& shaders);
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
    GpuPipeline _pipeline;
    GpuBufferHostCbv<Constants> _constants;
    GpuBufferHostSrv<baked::SkinningVertex> _vertices;
    GpuBufferHostIndex<baked::Index> _indices;
    GpuBufferHostSrv<Float4x4> _joint_inverse_bind_buffer;
    GpuBufferHostSrv<Float4x4> _joint_global_transform_buffer;
    GpuTextureSrv _texture;
    float _animation_time = 0.0f;
    float _animation_duration = 0.0f;
    std::vector<Float4x4> _node_global_transforms;
    baked::AnimationMesh _animation_mesh;
};

} // namespace fb::demos::anim
