#include "half.hpp"

#pragma warning(push)
#pragma warning(disable : 4244)
#define FP16_USE_NATIVE_CONVERSION 1
#include <fp16.h>
#pragma warning(pop)

namespace fb {

auto half_from_float(float f) -> uint16_t {
    return fp16_ieee_from_fp32_value(f);
}

auto float_from_half(uint16_t h) -> float {
    return fp16_ieee_to_fp32_value(h);
}

} // namespace fb
