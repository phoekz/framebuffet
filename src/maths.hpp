#pragma once

#include <pch.hpp>

namespace fb {

using Rectangle = DirectX::SimpleMath::Rectangle;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Plane = DirectX::SimpleMath::Plane;
using Quaternion = DirectX::SimpleMath::Quaternion;
using ColorRgba32f = DirectX::SimpleMath::Color;
using ColorRgba8 = DirectX::PackedVector::XMUBYTEN4;
using Ray = DirectX::SimpleMath::Ray;
using Viewport = DirectX::SimpleMath::Viewport;

constexpr float PI = DirectX::XM_PI;
constexpr auto rad_from_deg(float deg) -> float {
    return deg * PI / 180.0f;
}
constexpr auto deg_from_rad(float rad) -> float {
    return rad * 180.0f / PI;
}

}  // namespace fb
