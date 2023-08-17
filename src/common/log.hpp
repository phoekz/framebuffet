#pragma once

#include "pch.hpp"

namespace fb {

auto output_debug_string(std::string_view str) -> void;
auto output_console_string(std::string_view str) -> void;

#define FB_LOG_INFO(...) fb::log_detail::log_info(__VA_ARGS__);
#define FB_LOG_WARN(...) fb::log_detail::log_warn(__VA_ARGS__);
#define FB_LOG_ERROR(...) fb::log_detail::log_error(__VA_ARGS__);

namespace log_detail {

    enum class LogLevel {
        Info,
        Warn,
        Error,
    };

    inline constexpr auto log_level_name(LogLevel level) -> std::string_view {
        switch (level) {
            using enum LogLevel;
            case Info: return "Info"sv;
            case Warn: return "Warn"sv;
            case Error: return "Error"sv;
            default: return "Unknown"sv;
        }
    }

    template<typename... Args>
    inline auto log(LogLevel level, std::string_view fmt, Args&&... args) -> void {
        const auto now = std::chrono::system_clock::now();
        const auto level_str = log_level_name(level);
        const auto str = std::vformat(fmt, std::make_format_args(args...));
        const auto output = std::format("[{}] [{}] - {}\n", now, level_str, str);
        output_console_string(output);
        output_debug_string(output);
    }

    template<typename... Args>
    auto log_info(std::string_view fmt, Args&&... args) -> void {
        log(LogLevel::Info, fmt, args...);
    }

    template<typename... Args>
    auto log_warn(std::string_view fmt, Args&&... args) -> void {
        log(LogLevel::Warn, fmt, args...);
    }

    template<typename... Args>
    auto log_error(std::string_view fmt, Args&&... args) -> void {
        log(LogLevel::Error, fmt, args...);
    }

}  // namespace log_detail

}  // namespace fb
