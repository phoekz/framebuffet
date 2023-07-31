#pragma once

#include <cstdint>

namespace fb {

class pcg {
  public:
    auto random_uint() -> uint32_t;
    auto random_float() -> float;

  private:
    uint64_t state = 0x853c49e6748fea9b;
    uint64_t inc = 0xda3e39cb94b95bdb;
};

}  // namespace fb
