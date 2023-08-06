#pragma once

#include <pch.hpp>
#include <log.hpp>
#include <error.hpp>

namespace fb {

//
// File I/O.
//

auto read_whole_file(std::string_view path) -> std::vector<std::byte>;

//
// String conversions.
//

auto from_wstr(std::wstring_view wstr) -> std::string;
auto to_wstr(std::string_view str) -> std::wstring;

//
// Frame timing.
//

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

//
// COM helpers.
//

template<typename T>
using ComPtr = wil::com_ptr_nothrow<T>;

//
// Macros.
//

#define FB_NO_COPY_MOVE(T) \
    T(const T&) = delete; \
    T(T&&) = delete; \
    auto operator=(const T&)->T& = delete; \
    auto operator=(T&&)->T& = delete;

}  // namespace fb
