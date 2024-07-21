#include "camera.hpp"

namespace fb::camera {

auto Camera::create(const CreateDesc& desc) -> void {
    _aspect_ratio = desc.aspect_ratio;
    _fov = desc.fov;
    _near_clip = desc.near_clip;
    _far_clip = desc.far_clip;

    _spherical = float3(0.0f, 0.5f * PI, desc.distance_from_origin);
    _origin = float3::Zero;
    _up = float3::Up;

    _target_spherical = _spherical;
    _target_origin = _origin;
    _target_spherical_rate = 10.0f;
    _target_origin_rate = 10.0f;

    _prev_cursor_position = float2::Zero;
}

auto Camera::gui(const GuiDesc&) -> void {}

static auto exponential_lerp(float a, float b, float rate, float dt) -> float {
    return std::lerp(b, a, std::exp2(-rate * dt));
}

auto Camera::update(const UpdateDesc& desc) -> void {
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
        _target_spherical.y = std::clamp(_target_spherical.y, phi_epsilon, PI - phi_epsilon);
    }

    // Clamp spherical radius.
    _target_spherical.z = std::clamp(_target_spherical.z, 2.0f * _near_clip, 0.5f * _far_clip);

    // Animation.
    auto* sph = (float*)&_spherical;
    auto* org = (float*)&_origin;
    auto* target_sph = (float*)&_target_spherical;
    auto* target_org = (float*)&_target_origin;
    for (size_t i = 0; i < 3; ++i) {
        sph[i] = exponential_lerp(sph[i], target_sph[i], _target_spherical_rate, delta_time);
        org[i] = exponential_lerp(org[i], target_org[i], _target_origin_rate, delta_time);
    }

    // Next frame.
    _prev_cursor_position = cursor_position;
}

auto Camera::clip_from_view() const -> float4x4 {
    return float4x4::CreatePerspectiveFieldOfView(_fov, _aspect_ratio, _near_clip, _far_clip);
}

auto Camera::view_from_world() const -> float4x4 {
    return float4x4::CreateLookAt(position(), float3::Zero, _up);
}

auto Camera::clip_from_world() const -> float4x4 {
    return view_from_world() * clip_from_view();
}

auto Camera::position() const -> float3 {
    float3 cartesian;
    cartesian.x = _spherical.z * std::sin(_spherical.y) * std::sin(_spherical.x);
    cartesian.y = _spherical.z * std::cos(_spherical.y);
    cartesian.z = _spherical.z * std::sin(_spherical.y) * std::cos(_spherical.x);
    return cartesian;
}

} // namespace fb::camera
