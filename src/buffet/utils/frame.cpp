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

auto Frame::create() -> void {
    _frequency = win32_get_frequency();
    _curr_time = win32_get_performance_counter();
}

auto Frame::update() -> void {
    // Update count.
    _count++;

    // Update timing values.
    {
        _prev_time = _curr_time;
        _curr_time = win32_get_performance_counter();
        _delta_time = _curr_time - _prev_time;
        _elapsed_time += _delta_time;
        _delta_time_sec = (float)((double)_delta_time / (double)_frequency);
        _elapsed_time_sec = (float)((double)_elapsed_time / (double)_frequency);
    }

    // Update delta time history ring buffer.
    {
        _delta_time_history[_delta_time_history_index] = _delta_time_sec;
        _delta_time_history_index = (_delta_time_history_index + 1) % MAX_DELTA_TIME_HISTORY;
    }

    // Update smoothed delta time.
    {
        float sum = 0.0f;
        for (float dt : _delta_time_history) {
            sum += dt;
        }
        _smoothed_delta_time = sum / (float)MAX_DELTA_TIME_HISTORY;
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
