#include "pcg.hpp"
#include <cmath>

namespace fb {

uint32_t pcg::random_uint() {
#pragma warning(push)
// conversion from 'uint64_t' to 'uint32_t', possible loss of data
#pragma warning(disable : 4244)
// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable : 4146)

    uint64_t oldstate = state;
    state = oldstate * 6364136223846793005ULL + inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));

#pragma warning(pop)
}

float pcg::random_float() {
    return (float)std::ldexp(random_uint(), -32);
}

}  // namespace fb
