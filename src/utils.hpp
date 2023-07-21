#pragma once

#include <source_location>
#include <format>
#include <string_view>
#include <string>

#include <windows.h>

namespace fb {

enum class LogLevel {
    Info,
    Warn,
    Error,
};

const char* log_level_name(LogLevel level);

template<typename... Args>
void log_internal(
    LogLevel level,
    const std::source_location& loc,
    std::string_view fmt,
    Args&&... args) noexcept {
    std::string str = std::vformat(fmt, std::make_format_args(args...));
    std::string dbg_str =
        std::format("[{}]: {}({}): {}\n", log_level_name(level), loc.file_name(), loc.line(), str);
    OutputDebugStringA(dbg_str.c_str());
}

std::string from_wstr(std::wstring_view wstr) noexcept;

}  // namespace fb

#define log_info(fmt, ...) \
    fb::log_internal(fb::LogLevel::Info, std::source_location::current(), fmt, __VA_ARGS__)
#define log_warn(fmt, ...) \
    fb::log_internal(fb::LogLevel::Warn, std::source_location::current(), fmt, __VA_ARGS__)
#define log_error(fmt, ...) \
    fb::log_internal(fb::LogLevel::Error, std::source_location::current(), fmt, __VA_ARGS__)
