#pragma once

#include "pch.hpp"

namespace fb {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

class Instant {
  public:
    Instant();

    auto elapsed_time() const -> double;

  private:
    TimePoint _time_point;
};

} // namespace fb
