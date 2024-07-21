#pragma once

#include "pch.hpp"

namespace fb {

using Quaternion = DirectX::SimpleMath::Quaternion;
using Rectangle = DirectX::SimpleMath::Rectangle;
using Plane = DirectX::SimpleMath::Plane;
using RgbaFloat = DirectX::SimpleMath::Color;
using RgbaByte = DirectX::PackedVector::XMUBYTEN4;
using Ray = DirectX::SimpleMath::Ray;
using Viewport = DirectX::SimpleMath::Viewport;

inline constexpr float PI = DirectX::XM_PI;

inline constexpr RgbaByte COLOR_RED = {(uint8_t)255, 0, 0, 255};
inline constexpr RgbaByte COLOR_GREEN = {(uint8_t)0, 255, 0, 255};
inline constexpr RgbaByte COLOR_BLUE = {(uint8_t)0, 0, 255, 255};
inline constexpr RgbaByte COLOR_YELLOW = {(uint8_t)255, 255, 0, 255};
inline constexpr RgbaByte COLOR_MAGENTA = {(uint8_t)255, 0, 255, 255};
inline constexpr RgbaByte COLOR_CYAN = {(uint8_t)0, 255, 255, 255};
inline constexpr RgbaByte COLOR_WHITE = {(uint8_t)255, 255, 255, 255};
inline constexpr RgbaByte COLOR_BLACK = {(uint8_t)0, 0, 0, 255};

inline constexpr auto rad_from_deg(float deg) -> float {
    return deg * PI / 180.0f;
}

inline constexpr auto deg_from_rad(float rad) -> float {
    return rad * 180.0f / PI;
}

inline constexpr auto dir_from_lonlat(float lon, float lat) -> float3 {
    return {
        std::cos(lat) * std::cos(lon),
        std::sin(lat),
        std::cos(lat) * std::sin(lon),
    };
}

inline constexpr auto
float4x4_from_trs(const float3& t, const Quaternion& r, const float3& s) -> float4x4 {
    // Note: inlined quaternion-float3x3 conversion.
    float4x4 m;
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

inline auto rgb_from_hsv(float3 hsv) -> float3 {
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    if (h < 60.0f) {
        return float3(c + m, x + m, m);
    } else if (h < 120.0f) {
        return float3(x + m, c + m, m);
    } else if (h < 180.0f) {
        return float3(m, c + m, x + m);
    } else if (h < 240.0f) {
        return float3(m, x + m, c + m);
    } else if (h < 300.0f) {
        return float3(x + m, m, c + m);
    } else {
        return float3(c + m, m, x + m);
    }
}

inline constexpr auto mip_count_from_size(uint width, uint height) -> uint {
    uint mip_count = 1;
    while (width > 1 || height > 1) {
        width = std::max(1u, width / 2);
        height = std::max(1u, height / 2);
        mip_count++;
    }
    return mip_count;
}

inline constexpr auto mip_count_from_size(uint2 size) -> uint {
    return mip_count_from_size(size.x, size.y);
}

} // namespace fb

//
// Formatters.
//

template<>
struct std::formatter<fb::float2>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::float2 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f})", v.x, v.y);
    }
};

template<>
struct std::formatter<fb::float3>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::float3 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<fb::float4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::float4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({:6.3f} {:6.3f} {:6.3f} {:6.3f})", v.x, v.y, v.z, v.w);
    }
};

template<>
struct std::formatter<fb::float4x4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::float4x4 m, FormatContext& fc) const {
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
struct std::formatter<fb::uint2>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::uint2 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {})", v.x, v.y);
    }
};

template<>
struct std::formatter<fb::uint3>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::uint3 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {} {})", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<fb::uint4>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::uint4 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "({} {} {} {})", v.x, v.y, v.z, v.w);
    }
};
