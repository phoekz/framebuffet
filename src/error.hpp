#pragma once

#include <pch.hpp>
#include <log.hpp>

namespace fb {

namespace error_detail {

    inline auto hr_message(HRESULT hr) -> std::string {
        _com_error err(hr);
        return std::string {err.ErrorMessage()};
    }

    inline auto terminate() -> void {
        std::terminate();
    }

}  // namespace error_detail

#define FB_FATAL() fb::error_detail::terminate()

#define FB_ASSERT(condition) \
    do { \
        if (!(condition)) [[unlikely]] { \
            auto __loc = std::source_location::current(); \
            auto __msg = std::format( \
                "Assertion failed: {}\nLocation: {}:{}", \
                #condition, \
                __loc.file_name(), \
                __loc.line()); \
            FB_INTERNAL_POST_ASSERTION(__msg); \
        } \
    } while (false)

#define FB_ASSERT_MSG(condition, ...) \
    do { \
        if (!(condition)) [[unlikely]] { \
            auto __loc = std::source_location::current(); \
            auto __msg = std::format( \
                "Assertion failed: {}\nLocation: {}:{}\nMessage: {}", \
                #condition, \
                __loc.file_name(), \
                __loc.line(), \
                std::format(__VA_ARGS__)); \
            FB_INTERNAL_POST_ASSERTION(__msg); \
        } \
    } while (false)

#define FB_ASSERT_HR(hr) \
    do { \
        if (FAILED(hr)) [[unlikely]] { \
            auto __loc = std::source_location::current(); \
            auto __msg = std::format( \
                "HRESULT failed: {}\nLocation: {}:{}", \
                fb::error_detail::hr_message(hr), \
                __loc.file_name(), \
                __loc.line()); \
            FB_INTERNAL_POST_ASSERTION(__msg); \
        } \
    } while (false)

#define FB_INTERNAL_POST_ASSERTION(msg) \
    fb::output_debug_string(msg); \
    if (IsDebuggerPresent()) \
        __debugbreak(); \
    FB_FATAL();

}  // namespace fb
