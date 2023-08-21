#pragma once

#include "pch.hpp"

namespace fb {

auto half_from_float(float f) -> uint16_t;
auto float_from_half(uint16_t h) -> float;

} // namespace fb
