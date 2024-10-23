#pragma once

#include "rdc.hpp"

namespace fb {

struct RdcSample {
    float3 wi; // Incoming direction.
    float3 r;  // Reflectance.
    float pdf; // Probability density function.
};

FB_INLINE auto rdc_hemisphere_uniform_wi(float s, float t) -> float3 {
    const float u = FLOAT_PI * 2.0f * s;
    const float v = std::sqrt(std::max(0.0f, 1.0f - t * t));
    const float px = v * std::cos(u);
    const float py = t;
    const float pz = v * std::sin(u);
    return float3(px, py, pz);
}

FB_INLINE auto rdc_hemisphere_uniform_pdf() -> float {
    return 1.0f / (2.0f * FLOAT_PI);
}

FB_INLINE auto rdc_hemisphere_uniform_sample(float s, float t) -> RdcSample {
    const auto wi = rdc_hemisphere_uniform_wi(s, t);
    const auto r = float3(0.2f, 0.2f, 0.2f);
    const auto pdf = rdc_hemisphere_uniform_pdf();
    return RdcSample {
        .wi = wi,
        .r = r,
        .pdf = pdf,
    };
}

} // namespace fb
