#pragma once

#include "pch.hpp"

namespace fb {

//
// Constants.
//

inline constexpr float FLOAT_PI = glm::pi<float>();

inline constexpr float3 FLOAT3_ZERO = {0.0f, 0.0f, 0.0f};
inline constexpr float3 FLOAT3_ONE = {1.0f, 1.0f, 1.0f};
inline constexpr float3 FLOAT3_X = {1.0f, 0.0f, 0.0f};
inline constexpr float3 FLOAT3_Y = {0.0f, 1.0f, 0.0f};
inline constexpr float3 FLOAT3_Z = {0.0f, 0.0f, 1.0f};
inline constexpr float3 FLOAT3_UP = {0.0f, 1.0f, 0.0f};

inline constexpr float4x4 FLOAT4X4_IDENTITY = glm::identity<float4x4>();

inline constexpr float_quat FLOAT_QUAT_IDENTITY = glm::identity<float_quat>();

//
// Scalar functions.
//

FB_INLINE constexpr auto rad_from_deg(float deg) -> float {
    return deg * FLOAT_PI / 180.0f;
}

FB_INLINE constexpr auto deg_from_rad(float rad) -> float {
    return rad * 180.0f / FLOAT_PI;
}

FB_INLINE constexpr auto mip_count_from_size(uint width, uint height) -> uint {
    uint mip_count = 1;
    while (width > 1 || height > 1) {
        width = std::max(1u, width / 2);
        height = std::max(1u, height / 2);
        mip_count++;
    }
    return mip_count;
}

FB_INLINE constexpr auto mip_count_from_size(uint2 size) -> uint {
    return mip_count_from_size(size.x, size.y);
}

FB_INLINE constexpr auto float_lerp(float a, float b, float t) -> float {
    return a * (1.0f - t) + b * t;
}

FB_INLINE constexpr auto float_explerp(float a, float b, float rate, float dt) -> float {
    return float_lerp(b, a, std::exp2(-rate * dt));
}

FB_INLINE auto float_isfinite(float v) -> bool {
    return std::isfinite(v);
}

//
// Vector functions.
//

FB_INLINE auto float3_abs(const float3& v) -> float3 {
    return float3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

FB_INLINE auto float3_argmax(const float3& v) -> uint {
    return v.z > v.y ? (v.z > v.x ? 2u : 0u) : (v.y > v.x ? 1u : 0u);
}

FB_INLINE auto float3_normalize(const float3& v) -> float3 {
    return glm::normalize<3, float, glm::highp>(v);
}

FB_INLINE auto float3_dot(const float3& a, const float3& b) -> float {
    return glm::dot<3, float, glm::highp>(a, b);
}

FB_INLINE auto float3_cross(const float3& a, const float3& b) -> float3 {
    return glm::cross<float, glm::highp>(a, b);
}

FB_INLINE auto float3_lerp(const float3& a, const float3& b, float t) -> float3 {
    return glm::lerp<float, glm::highp>(a, b, t);
}

FB_INLINE auto float3_distance(const float3& a, const float3& b) -> float {
    return glm::distance<3, float, glm::highp>(a, b);
}

FB_INLINE constexpr auto float3_from_lonlat(float lon, float lat) -> float3 {
    return {
        std::cos(lat) * std::cos(lon),
        std::sin(lat),
        std::cos(lat) * std::sin(lon),
    };
}

//
// Matrix functions.
//

FB_INLINE auto float3x3_transpose(const float3x3& m) -> float3x3 {
    return glm::transpose<3, 3, float, glm::highp>(m);
}

FB_INLINE auto float4x4_inverse(const float4x4& m) -> float4x4 {
    return glm::inverse<4, 4, float, glm::highp>(m);
}

FB_INLINE auto float4x4_translation(float x, float y, float z) -> float4x4 {
    return glm::translate<float, glm::highp>(float4x4(1.0f), {x, y, z});
}

FB_INLINE auto float4x4_translation(const float3& t) -> float4x4 {
    return glm::translate<float>(float4x4(1.0f), t);
}

FB_INLINE auto float4x4_rotation_x(float angle) -> float4x4 {
    return glm::rotate<float, glm::highp>(float4x4(1.0f), angle, FLOAT3_X);
}

FB_INLINE auto float4x4_rotation_y(float angle) -> float4x4 {
    return glm::rotate<float, glm::highp>(float4x4(1.0f), angle, FLOAT3_Y);
}

FB_INLINE auto float4x4_rotation_z(float angle) -> float4x4 {
    return glm::rotate<float, glm::highp>(float4x4(1.0f), angle, FLOAT3_Z);
}

FB_INLINE auto float4x4_scaling(float x, float y, float z) -> float4x4 {
    return glm::scale<float, glm::highp>(float4x4(1.0f), {x, y, z});
}

FB_INLINE auto float4x4_scaling(const float3& s) -> float4x4 {
    return glm::scale<float, glm::highp>(float4x4(1.0f), s);
}

FB_INLINE auto
float4x4_lookat(const float3& eye, const float3& center, const float3& up) -> float4x4 {
    return glm::lookAt<float, glm::highp>(eye, center, up);
}

FB_INLINE auto
float4x4_orthographic(float left, float right, float bottom, float top, float z_near, float z_far)
    -> float4x4 {
    return glm::ortho<float>(left, right, bottom, top, z_near, z_far);
}

FB_INLINE auto
float4x4_perspective(float fovy, float aspect, float z_near, float z_far) -> float4x4 {
    return glm::perspective<float>(fovy, aspect, z_near, z_far);
}

FB_INLINE auto
float4x4_from_trs(const float3& t, const float_quat& r, const float3& s) -> float4x4 {
    // Note: inlined quaternion-float3x3 conversion.
    float4x4 m;
    float rxx = r.x * r.x;
    float ryy = r.y * r.y;
    float rzz = r.z * r.z;
    m[0][0] = s.x * (1.0f - 2.0f * ryy - 2.0f * rzz);
    m[0][1] = (2.0f * r.x * r.y + 2.0f * r.z * r.w);
    m[0][2] = (2.0f * r.x * r.z - 2.0f * r.y * r.w);
    m[0][3] = 0.0f;
    m[1][0] = (2.0f * r.x * r.y - 2.0f * r.z * r.w);
    m[1][1] = s.y * (1.0f - 2.0f * rxx - 2.0f * rzz);
    m[1][2] = (2.0f * r.y * r.z + 2.0f * r.x * r.w);
    m[1][3] = 0.0f;
    m[2][0] = (2.0f * r.x * r.z + 2.0f * r.y * r.w);
    m[2][1] = (2.0f * r.y * r.z - 2.0f * r.x * r.w);
    m[2][2] = s.z * (1.0f - 2.0f * rxx - 2.0f * ryy);
    m[2][3] = 0.0f;
    m[3][0] = t.x;
    m[3][1] = t.y;
    m[3][2] = t.z;
    m[3][3] = 1.0f;
    return m;
}

FB_INLINE constexpr auto float4x4_transform_point(const float4x4& t, const float3& p) -> float3 {
    return float3(t * float4(p, 1.0f));
}

FB_INLINE constexpr auto float4x4_transform_vector(const float4x4& t, const float3& v) -> float3 {
    return float3(t * float4(v, 0.0f));
}

//
// Quaternion functions.
//

FB_INLINE auto float_quat_slerp(const float_quat& a, const float_quat& b, float t) -> float_quat {
    return glm::slerp(a, b, t);
}

//
// Color functions.
//

FB_INLINE auto rgb_from_hsv(float3 hsv) -> float3 {
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    if (h < 60.0f) {
        return {c + m, x + m, m};
    } else if (h < 120.0f) {
        return {x + m, c + m, m};
    } else if (h < 180.0f) {
        return {m, c + m, x + m};
    } else if (h < 240.0f) {
        return {m, x + m, c + m};
    } else if (h < 300.0f) {
        return {x + m, m, c + m};
    } else {
        return {c + m, m, x + m};
    }
}

//
// Rectangle.
//

struct float_rect {
    float2 min;
    float2 max;

    float_rect(float x, float y, float w, float h)
        : min(x, y)
        , max(x + w, y + h) {}
};

FB_INLINE auto float_rect_center(const float_rect& r) -> float2 {
    return (r.min + r.max) * 0.5f;
}

FB_INLINE auto float_rect_size(const float_rect& r) -> float2 {
    return r.max - r.min;
}

FB_INLINE auto float_rect_contains(const float_rect& r, float x, float y) -> bool {
    return x >= r.min.x && x <= r.max.x && y >= r.min.y && y <= r.max.y;
}

FB_INLINE auto float_rect_contains(const float_rect& r, float2 p) -> bool {
    return p.x >= r.min.x && p.x <= r.max.x && p.y >= r.min.y && p.y <= r.max.y;
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
            m[0][0],
            m[1][0],
            m[2][0],
            m[3][0],
            m[0][1],
            m[1][1],
            m[2][1],
            m[3][1],
            m[0][2],
            m[1][2],
            m[2][2],
            m[3][2],
            m[0][3],
            m[1][3],
            m[2][3],
            m[3][3]
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
