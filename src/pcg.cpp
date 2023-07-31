#include "pcg.hpp"
#include <cmath>

#pragma warning(push)
#pragma warning(disable : 4244)  // conversion from 'uint64_t' to 'uint32_t', possible loss of data
#pragma warning(disable : 4146)  // unary minus operator applied to unsigned type

namespace fb {

auto pcg::random_uint() -> uint32_t {
    uint64_t oldstate = state;
    state = oldstate * 6364136223846793005ULL + inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

auto pcg::random_float() -> float {
    return (float)std::ldexp(random_uint(), -32);
}

}  // namespace fb

#pragma warning(pop)
