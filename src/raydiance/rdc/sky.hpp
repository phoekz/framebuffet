#pragma once

#include "rdc.hpp"

namespace fb {

struct RdcSkyParams {
    float elevation; // The solar elevation (azimuth) angle, in radians. Must be in `[0, π/2]`.
    float turbidity; // Turbidity must be in `[1, 10]`.
    float albedo[3]; // Ground albedo must be in `[0, 1]`.
};

struct RdcSkyState {
    float params[27];
    float sky_radiances[3];
};

enum class RdcSkyStateResult : uint {
    Success,
    ElevationOutOfRange,
    TurbidityOutOfRange,
    AlbedoOutOfRange,
};

enum class RdcSkyChannel : uint {
    R = 0,
    G,
    B
};

auto rdc_sky_create(RdcSkyState& state, const RdcSkyParams& params) -> RdcSkyStateResult;
auto rdc_sky_radiance(const RdcSkyState& state, float theta, float gamma, RdcSkyChannel channel)
    -> float;

} // namespace fb
