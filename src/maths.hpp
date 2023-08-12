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

using Uint2 = DirectX::XMUINT2;
using Uint3 = DirectX::XMUINT3;
using Uint4 = DirectX::XMUINT4;

constexpr float PI = DirectX::XM_PI;

constexpr auto rad_from_deg(float deg) -> float {
    return deg * PI / 180.0f;
}

constexpr auto deg_from_rad(float rad) -> float {
    return rad * 180.0f / PI;
}

constexpr auto dir_from_lonlat(float lon, float lat) -> Vector3 {
    return {
        std::cos(lat) * std::cos(lon),
        std::sin(lat),
        std::cos(lat) * std::sin(lon),
    };
}

}  // namespace fb

//
// Formatters.
//

template<>
struct std::formatter<fb::Vector3>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Vector3 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<fb::Vector4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Vector4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z, v.w);
    }
};

template<>
struct std::formatter<fb::Matrix>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Matrix m, FormatContext& fc) const {
        return std::format_to(
            fc.out(),
            "\n|{:6.3f} {:6.3f} {:6.3f} {:6.3f}|"
            "\n|{:6.3f} {:6.3f} {:6.3f} {:6.3f}|"
            "\n|{:6.3f} {:6.3f} {:6.3f} {:6.3f}|"
            "\n|{:6.3f} {:6.3f} {:6.3f} {:6.3f}|"
            "",
            m.m[0][0],
            m.m[1][0],
            m.m[2][0],
            m.m[3][0],
            m.m[0][1],
            m.m[1][1],
            m.m[2][1],
            m.m[3][1],
            m.m[0][2],
            m.m[1][2],
            m.m[2][2],
            m.m[3][2],
            m.m[0][3],
            m.m[1][3],
            m.m[2][3],
            m.m[3][3]);
    }
};

template<>
struct std::formatter<fb::Uint4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Uint4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {} {} {})", v.x, v.y, v.z, v.w);
    }
};
