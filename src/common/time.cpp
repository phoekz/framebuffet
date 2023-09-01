#include "time.hpp"

#include "pch.hpp"

namespace fb {

Instant::Instant() {
    _prev = win32_get_performance_counter();
}

auto Instant::elapsed_time() const -> double {
    const auto freq = win32_get_frequency();
    const auto curr = win32_get_performance_counter();
    const auto elapsed = curr - _prev;
    const auto seconds = (double)elapsed / (double)freq;
    return seconds;
}

auto win32_get_frequency() -> uint64_t {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

auto win32_get_performance_counter() -> uint64_t {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

} // namespace fb
