#pragma once

#include "pch.hpp"

namespace fb {

class Pcg {
public:
    auto random_uint() -> uint32_t;
    auto random_float() -> float;

private:
    uint64_t _state = 0x853c49e6748fea9b;
    uint64_t _inc = 0xda3e39cb94b95bdb;
};

} // namespace fb
