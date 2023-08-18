#include "frame.hpp"

namespace fb {

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

Frame::Frame() {
    _frequency = win32_get_frequency();
    _curr_time = win32_get_performance_counter();
}

auto Frame::update() -> void {
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

} // namespace fb
