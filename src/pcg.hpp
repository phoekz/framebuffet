#pragma once

#include <cstdint>

namespace fb {

struct pcg {
    uint32_t random_uint();
    float random_float();

    uint64_t state = 0x853c49e6748fea9b;
    uint64_t inc = 0xda3e39cb94b95bdb;
};

}  // namespace fb
