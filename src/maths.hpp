#pragma once

#include <directxtk12/SimpleMath.h>

namespace fb {

using Rect = DirectX::SimpleMath::Rectangle;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Mat4x4 = DirectX::SimpleMath::Matrix;
using Plane = DirectX::SimpleMath::Plane;
using Quat = DirectX::SimpleMath::Quaternion;
using Color = DirectX::SimpleMath::Color;
using Ray = DirectX::SimpleMath::Ray;
using Viewport = DirectX::SimpleMath::Viewport;

constexpr float PI = DirectX::XM_PI;
constexpr float rad_from_deg(float deg) {
    return deg * PI / 180.0f;
}
constexpr float deg_from_rad(float rad) {
    return rad * 180.0f / PI;
}

}  // namespace fb
