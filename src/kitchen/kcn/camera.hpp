#pragma once

#include <common/common.hpp>

namespace fb {

struct KcnCameraCreateDesc {
    float aspect_ratio = 1.0f;
    float fov = rad_from_deg(60.0f);
    float near_clip = 0.1f;
    float far_clip = 100.0f;
    float distance_from_origin = 2.5f;
};

struct KcnCameraGuiDesc {};

struct KcnCameraUpdateDesc {
    float delta_time;
    float2 cursor_position;
    bool mouse_left;
    float mouse_wheel_y;
};

class KcnCamera {
public:
    auto create(const KcnCameraCreateDesc& desc) -> void;
    auto gui(const KcnCameraGuiDesc& desc) -> void;
    auto update(const KcnCameraUpdateDesc& desc) -> void;

    auto aspect_ratio() const -> float { return _aspect_ratio; }
    auto fov() const -> float { return _fov; }
    auto near_clip() const -> float { return _near_clip; }
    auto far_clip() const -> float { return _far_clip; }
    auto clip_from_view() const -> float4x4;
    auto view_from_world() const -> float4x4;
    auto clip_from_world() const -> float4x4;
    auto position() const -> float3;

private:
    float _aspect_ratio;
    float _fov;
    float _near_clip;
    float _far_clip;

    float3 _spherical;
    float3 _origin;
    float3 _up;

    float3 _target_spherical;
    float3 _target_origin;
    float _target_spherical_rate;
    float _target_origin_rate;

    float2 _prev_cursor_position;
};

} // namespace fb
