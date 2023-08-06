#pragma once

#include <pch.hpp>

namespace fb {

class Instant {
  public:
    Instant();

    auto elapsed_time() const -> double;

  private:
    std::chrono::high_resolution_clock::time_point _time_point;
};

class FrameTiming {
  public:
    FrameTiming();

    auto update() -> void;
    auto delta_time() const -> float;
    auto elapsed_time() const -> float;

  private:
    uint64_t m_frequency;
    uint64_t m_prev_time;
    uint64_t m_curr_time;
    uint64_t m_delta_time;
    uint64_t m_elapsed_time;
    float m_delta_time_sec;
    float m_elapsed_time_sec;
};

}  // namespace fb
