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
    static constexpr size_t MAX_DELTA_TIME_HISTORY = 128;
    static constexpr float DELTA_TIME_UPDATE_INTERVAL = 1.0f / 8.0f;

    FrameTiming();

    auto update() -> void;
    auto delta_time() const -> float { return _delta_time_sec; }
    auto smoothed_delta_time() const -> float { return _smoothed_delta_time; }
    auto elapsed_time() const -> float { return _elapsed_time_sec; }
    auto last_delta_time() const -> float { return _last_delta_time; }
    auto last_fps() const -> float {
        return _last_delta_time != 0.0f ? 1.0f / _last_delta_time : 0.0f;
    }

  private:
    uint64_t _frequency;
    uint64_t _prev_time;
    uint64_t _curr_time;
    uint64_t _delta_time;
    uint64_t _elapsed_time;
    float _delta_time_sec;
    float _elapsed_time_sec;
    std::deque<float> _delta_time_history;
    float _smoothed_delta_time;
    float _since_last_delta_time_update;
    float _last_delta_time;
};

}  // namespace fb
