#pragma once

#include "pch.hpp"

namespace fb {

class Instant {
public:
    Instant();

    auto elapsed_time() const -> double;

private:
    uint64_t _prev;
};

auto win32_get_frequency() -> uint64_t;
auto win32_get_performance_counter() -> uint64_t;

} // namespace fb
