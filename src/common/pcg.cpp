#include "pcg.hpp"
#include "pch.hpp"

namespace fb {

#pragma warning(push)
#pragma warning(disable : 4244) // conversion from 'uint64_t' to 'uint', possible loss of data
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type

auto Pcg::random_uint() -> uint {
    uint64_t oldstate = _state;
    _state = oldstate * 6364136223846793005ULL + _inc;
    uint xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

auto Pcg::random_float() -> float {
    return (float)std::ldexp(random_uint(), -32);
}

#pragma warning(pop)

} // namespace fb
