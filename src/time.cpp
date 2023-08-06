#include "time.hpp"

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
    m_frequency = win32_get_frequency();
    m_curr_time = win32_get_performance_counter();
    m_prev_time = m_curr_time;
    m_delta_time = 0;
    m_elapsed_time = 0;
    m_delta_time_sec = 0.0f;
    m_elapsed_time_sec = 0.0f;
}

auto FrameTiming::update() -> void {
    m_prev_time = m_curr_time;
    m_curr_time = win32_get_performance_counter();
    m_delta_time = m_curr_time - m_prev_time;
    m_elapsed_time += m_delta_time;
    m_delta_time_sec = (float)((double)m_delta_time / (double)m_frequency);
    m_elapsed_time_sec = (float)((double)m_elapsed_time / (double)m_frequency);
}

auto FrameTiming::delta_time() const -> float {
    return m_delta_time_sec;
}

auto FrameTiming::elapsed_time() const -> float {
    return m_elapsed_time_sec;
}

}  // namespace fb
