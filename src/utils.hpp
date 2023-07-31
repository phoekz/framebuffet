#pragma once

#include <source_location>
#include <format>
#include <string_view>
#include <string>
#include <vector>
#include <sstream>

#define WIL_SUPPRESS_EXCEPTIONS
#include <wil/com.h>
#include <wil/resource.h>

namespace fb {

//
// File I/O.
//

auto read_whole_file(std::string_view path) -> std::vector<std::byte>;

//
// Logging.
//

enum class LogLevel {
    Info,
    Warn,
    Error,
};
auto log_level_name(LogLevel level) -> std::string_view;
auto log_output_debug_string(std::string_view str) -> void;
template<typename... Args>
auto log_internal(
    LogLevel level,
    const std::source_location& loc,
    std::string_view fmt,
    Args&&... args) -> void {
    std::string str = std::vformat(fmt, std::make_format_args(args...));
    std::string dbg_str =
        std::format("[{}]: {}({}): {}\n", log_level_name(level), loc.file_name(), loc.line(), str);
    log_output_debug_string(dbg_str);
}

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

}  // namespace fb

//
// Logging.
//

#define log_info(fmt, ...) \
    fb::log_internal(fb::LogLevel::Info, std::source_location::current(), fmt, __VA_ARGS__)
#define log_warn(fmt, ...) \
    fb::log_internal(fb::LogLevel::Warn, std::source_location::current(), fmt, __VA_ARGS__)
#define log_error(fmt, ...) \
    fb::log_internal(fb::LogLevel::Error, std::source_location::current(), fmt, __VA_ARGS__)
