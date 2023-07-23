#pragma once

#include <source_location>
#include <format>
#include <string_view>
#include <string>

namespace fb {

//
// Logging.
//

enum class LogLevel {
    Info,
    Warn,
    Error,
};

const char* log_level_name(LogLevel level);

void log_output_debug_string(std::string_view str) noexcept;

template<typename... Args>
void log_internal(
    LogLevel level,
    const std::source_location& loc,
    std::string_view fmt,
    Args&&... args) noexcept {
    std::string str = std::vformat(fmt, std::make_format_args(args...));
    std::string dbg_str =
        std::format("[{}]: {}({}): {}\n", log_level_name(level), loc.file_name(), loc.line(), str);
    log_output_debug_string(dbg_str);
}

//
// String conversions.
//

std::string from_wstr(std::wstring_view wstr) noexcept;

//
// Frame timing.
//

class FrameTiming {
  public:
    FrameTiming() noexcept;
    void update() noexcept;
    float delta_time() const noexcept;
    float elapsed_time() const noexcept;

  private:
    uint64_t m_frequency;
    uint64_t m_prev_time;
    uint64_t m_curr_time;
    uint64_t m_delta_time;
    uint64_t m_elapsed_time;
    float m_delta_time_sec;
    float m_elapsed_time_sec;
};

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
