#pragma once

#include "pch.hpp"

namespace fb {

using Rectangle = DirectX::SimpleMath::Rectangle;
using Float2 = DirectX::SimpleMath::Vector2;
using Float3 = DirectX::SimpleMath::Vector3;
using Float4 = DirectX::SimpleMath::Vector4;
using Float4x4 = DirectX::SimpleMath::Matrix;
using Plane = DirectX::SimpleMath::Plane;
using Quaternion = DirectX::SimpleMath::Quaternion;
using RgbaFloat = DirectX::SimpleMath::Color;
using RgbaByte = DirectX::PackedVector::XMUBYTEN4;
using Ray = DirectX::SimpleMath::Ray;
using Viewport = DirectX::SimpleMath::Viewport;

using Uint2 = DirectX::XMUINT2;
using Uint3 = DirectX::XMUINT3;
using Uint4 = DirectX::XMUINT4;

inline constexpr float PI = DirectX::XM_PI;

inline constexpr auto rad_from_deg(float deg) -> float {
    return deg * PI / 180.0f;
}

inline constexpr auto deg_from_rad(float rad) -> float {
    return rad * 180.0f / PI;
}

inline constexpr auto dir_from_lonlat(float lon, float lat) -> Float3 {
    return {
        std::cos(lat) * std::cos(lon),
        std::sin(lat),
        std::cos(lat) * std::sin(lon),
    };
}

inline constexpr auto float4x4_from_trs(const Float3& t, const Quaternion& r, const Float3& s)
    -> Float4x4 {
    // Note: inlined quaternion-float3x3 conversion.
    Float4x4 m;
    float rxx = r.x * r.x;
    float ryy = r.y * r.y;
    float rzz = r.z * r.z;
    m.m[0][0] = s.x * (1.0f - 2.0f * ryy - 2.0f * rzz);
    m.m[0][1] = (2.0f * r.x * r.y + 2.0f * r.z * r.w);
    m.m[0][2] = (2.0f * r.x * r.z - 2.0f * r.y * r.w);
    m.m[0][3] = 0.0f;
    m.m[1][0] = (2.0f * r.x * r.y - 2.0f * r.z * r.w);
    m.m[1][1] = s.y * (1.0f - 2.0f * rxx - 2.0f * rzz);
    m.m[1][2] = (2.0f * r.y * r.z + 2.0f * r.x * r.w);
    m.m[1][3] = 0.0f;
    m.m[2][0] = (2.0f * r.x * r.z + 2.0f * r.y * r.w);
    m.m[2][1] = (2.0f * r.y * r.z - 2.0f * r.x * r.w);
    m.m[2][2] = s.z * (1.0f - 2.0f * rxx - 2.0f * ryy);
    m.m[2][3] = 0.0f;
    m.m[3][0] = t.x;
    m.m[3][1] = t.y;
    m.m[3][2] = t.z;
    m.m[3][3] = 1.0f;
    return m;
}

inline auto rgb_from_hsv(Float3 hsv) -> Float3 {
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    if (h < 60.0f) {
        return Float3(c + m, x + m, m);
    } else if (h < 120.0f) {
        return Float3(x + m, c + m, m);
    } else if (h < 180.0f) {
        return Float3(m, c + m, x + m);
    } else if (h < 240.0f) {
        return Float3(m, x + m, c + m);
    } else if (h < 300.0f) {
        return Float3(x + m, m, c + m);
    } else {
        return Float3(c + m, m, x + m);
    }
}

} // namespace fb

//
// Formatters.
//

template<>
struct std::formatter<fb::Float2>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Float2 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f})", v.x, v.y);
    }
};

template<>
struct std::formatter<fb::Float3>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Float3 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<fb::Float4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Float4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z, v.w);
    }
};

template<>
struct std::formatter<fb::Float4x4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Float4x4 m, FormatContext& fc) const {
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
            m.m[3][3]
        );
    }
};

template<>
struct std::formatter<fb::Uint2>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Uint2 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {})", v.x, v.y);
    }
};

template<>
struct std::formatter<fb::Uint3>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Uint3 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {} {})", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<fb::Uint4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Uint4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {} {} {})", v.x, v.y, v.z, v.w);
    }
};
