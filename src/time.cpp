#include "time.hpp"
#include "utils.hpp"

namespace fb {

Instant::Instant() {
    _time_point = std::chrono::high_resolution_clock::now();
}

auto Instant::elapsed_time() const -> double {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _time_point);
    auto seconds = elapsed.count() / 1'000'000'000.0;
    return seconds;
}

//
// Frame timing.
//

static auto win32_get_frequency() -> uint64_t {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

static auto win32_get_performance_counter() -> uint64_t {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

FrameTiming::FrameTiming() {
    _frequency = win32_get_frequency();
    _curr_time = win32_get_performance_counter();
    _prev_time = _curr_time;
    _delta_time = 0;
    _elapsed_time = 0;
    _delta_time_sec = 0.0f;
    _elapsed_time_sec = 0.0f;
    _delta_time_history = std::deque<float>();
    _smoothed_delta_time = 0.0f;
    _since_last_delta_time_update = 0.0f;
    _last_delta_time = 0.0f;
}

auto FrameTiming::update() -> void {
    // Update timing values.
    {
        _prev_time = _curr_time;
        _curr_time = win32_get_performance_counter();
        _delta_time = _curr_time - _prev_time;
        _elapsed_time += _delta_time;
        _delta_time_sec = (float)((double)_delta_time / (double)_frequency);
        _elapsed_time_sec = (float)((double)_elapsed_time / (double)_frequency);
    }

    // Update delta time history.
    {
        _delta_time_history.push_back(_delta_time_sec);
        if (_delta_time_history.size() > MAX_DELTA_TIME_HISTORY) {
            _delta_time_history.pop_front();
        }
    }

    // Update smoothed delta time.
    if (!_delta_time_history.empty()) {
        float sum = 0.0f;
        for (auto dt : _delta_time_history)
            sum += dt;
        _smoothed_delta_time = sum / (float)_delta_time_history.size();
    }

    // Update FPS.
    {
        _since_last_delta_time_update += _delta_time_sec;
        if (_since_last_delta_time_update >= DELTA_TIME_UPDATE_INTERVAL) {
            _since_last_delta_time_update = 0.0f;
            _last_delta_time = _smoothed_delta_time;
        }
    }
}

}  // namespace fb
