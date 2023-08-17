#include "time.hpp"

#include "pch.hpp"

namespace fb {

Instant::Instant() {
    _time_point = Clock::now();
}

auto Instant::elapsed_time() const -> double {
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _time_point);
    auto seconds = elapsed.count() / 1'000'000'000.0;
    return seconds;
}

}  // namespace fb
