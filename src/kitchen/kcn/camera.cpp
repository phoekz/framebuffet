#include "camera.hpp"

namespace fb {

auto KcnCamera::create(const KcnCameraCreateDesc& desc) -> void {
    _aspect_ratio = desc.aspect_ratio;
    _fov = desc.fov;
    _near_clip = desc.near_clip;
    _far_clip = desc.far_clip;

    _spherical = float3(0.0f, 0.5f * FLOAT_PI, desc.distance_from_origin);
    _origin = FLOAT3_ZERO;
    _up = FLOAT3_UP;

    _target_spherical = _spherical;
    _target_origin = _origin;
    _target_spherical_rate = 10.0f;
    _target_origin_rate = 10.0f;

    _prev_cursor_position = float2(0.0f);
}

auto KcnCamera::gui(const KcnCameraGuiDesc&) -> void {}

auto KcnCamera::update(const KcnCameraUpdateDesc& desc) -> void {
    // Unpack.
    const auto delta_time = desc.delta_time;
    const auto cursor_position = desc.cursor_position;
    const auto mouse_left = desc.mouse_left;
    const auto mouse_wheel_y = desc.mouse_wheel_y;

    // Zooming.
    if (mouse_wheel_y > 0.0f) {
        _target_spherical.z *= 0.9f;
    } else if (mouse_wheel_y < 0.0f) {
        _target_spherical.z *= 1.1f;
    }

    // Orientation.
    if (mouse_left) {
        const auto delta_cursor_position = cursor_position - _prev_cursor_position;
        _target_spherical.x -= delta_cursor_position.x * delta_time;
        _target_spherical.y -= delta_cursor_position.y * delta_time;
    }

    // Clamp spherical phi.
    {
        const auto phi_epsilon = 1.0f / 1024.0f;
        _target_spherical.y = std::clamp(_target_spherical.y, phi_epsilon, FLOAT_PI - phi_epsilon);
    }

    // Clamp spherical radius.
    _target_spherical.z = std::clamp(_target_spherical.z, 2.0f * _near_clip, 0.5f * _far_clip);

    // Animation.
    auto* sph = (float*)&_spherical;
    auto* org = (float*)&_origin;
    const auto* target_sph = (const float*)&_target_spherical;
    const auto* target_org = (const float*)&_target_origin;
    for (size_t i = 0; i < 3; ++i) {
        sph[i] = float_explerp(sph[i], target_sph[i], _target_spherical_rate, delta_time);
        org[i] = float_explerp(org[i], target_org[i], _target_origin_rate, delta_time);
    }

    // Next frame.
    _prev_cursor_position = cursor_position;
}

auto KcnCamera::clip_from_view() const -> float4x4 {
    return float4x4_perspective(_fov, _aspect_ratio, _near_clip, _far_clip);
}

auto KcnCamera::view_from_world() const -> float4x4 {
    return float4x4_lookat(position(), FLOAT3_ZERO, _up);
}

auto KcnCamera::clip_from_world() const -> float4x4 {
    return view_from_world() * clip_from_view();
}

auto KcnCamera::position() const -> float3 {
    float3 cartesian;
    cartesian.x = _spherical.z * std::sin(_spherical.y) * std::sin(_spherical.x);
    cartesian.y = _spherical.z * std::cos(_spherical.y);
    cartesian.z = _spherical.z * std::sin(_spherical.y) * std::cos(_spherical.x);
    return cartesian;
}

} // namespace fb
