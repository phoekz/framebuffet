#pragma once

#include <common/common.hpp>

namespace fb {

class Frame {
public:
    static constexpr size_t MAX_DELTA_TIME_HISTORY = 128;
    static constexpr float DELTA_TIME_UPDATE_INTERVAL = 1.0f / 8.0f;

    auto create() -> void;
    auto update() -> void;
    auto count() const -> uint64_t { return _count; }
    auto delta_time() const -> float { return _delta_time_sec; }
    auto smoothed_delta_time() const -> float { return _smoothed_delta_time; }
    auto elapsed_time() const -> float { return _elapsed_time_sec; }
    auto delta_time_history() const -> Span<const float> {
        return {_delta_time_history.data(), _delta_time_history.size()};
    }
    auto delta_time_history_index() const -> size_t { return _delta_time_history_index; }
    auto last_delta_time() const -> float { return _last_delta_time; }
    auto last_fps() const -> float {
        return _last_delta_time != 0.0f ? 1.0f / _last_delta_time : 0.0f;
    }

private:
    uint64_t _count = 0;
    uint64_t _frequency = 0;
    uint64_t _prev_time = 0;
    uint64_t _curr_time = 0;
    uint64_t _delta_time = 0;
    uint64_t _elapsed_time = 0;
    float _delta_time_sec = 0.0f;
    float _elapsed_time_sec = 0.0f;
    std::array<float, MAX_DELTA_TIME_HISTORY> _delta_time_history = {};
    size_t _delta_time_history_index = 0;
    float _smoothed_delta_time = 0.0f;
    float _since_last_delta_time_update = 0.0f;
    float _last_delta_time = 0.0f;
};

} // namespace fb
