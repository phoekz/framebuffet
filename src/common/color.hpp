#pragma once

#include "pch.hpp"

namespace fb {

//
// Types.
//

using RgbaFloat = float4;
using RgbaByte = ubyte4;

//
// Constants.
//

inline constexpr RgbaByte COLOR_RED = {(uint8_t)255, 0, 0, 255};
inline constexpr RgbaByte COLOR_GREEN = {(uint8_t)0, 255, 0, 255};
inline constexpr RgbaByte COLOR_BLUE = {(uint8_t)0, 0, 255, 255};
inline constexpr RgbaByte COLOR_YELLOW = {(uint8_t)255, 255, 0, 255};
inline constexpr RgbaByte COLOR_MAGENTA = {(uint8_t)255, 0, 255, 255};
inline constexpr RgbaByte COLOR_CYAN = {(uint8_t)0, 255, 255, 255};
inline constexpr RgbaByte COLOR_WHITE = {(uint8_t)255, 255, 255, 255};
inline constexpr RgbaByte COLOR_BLACK = {(uint8_t)0, 0, 0, 255};

//
// Color spaces.
//

FB_INLINE auto srgb_from_linear(float linear) -> float {
    if (linear <= 0.0031308f) {
        return 12.92f * linear;
    } else {
        return 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;
    }
}

} // namespace fb
