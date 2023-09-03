#pragma once

#include "pch.hpp"
#include "log.hpp"
#include "trap.hpp"

namespace fb {

namespace error_detail {

    auto hr_message(HRESULT hr) -> std::string;
    [[noreturn]] auto terminate() -> void;

} // namespace error_detail

#define FB_FATAL() fb::error_detail::terminate()

#define FB_ASSERT(condition)                              \
    do {                                                  \
        if (!(condition)) [[unlikely]] {                  \
            FRAME_ALLOCATION_TRAP = false;                \
            auto __loc = std::source_location::current(); \
            auto __msg = std::format(                     \
                "Assertion failed: {}\nLocation: {}:{}",  \
                #condition,                               \
                __loc.file_name(),                        \
                __loc.line()                              \
            );                                            \
            FRAME_ALLOCATION_TRAP = true;                 \
            FB_INTERNAL_POST_ASSERTION(__msg);            \
        }                                                 \
    } while (false)

#define FB_ASSERT_MSG(condition, ...)                                 \
    do {                                                              \
        if (!(condition)) [[unlikely]] {                              \
            FRAME_ALLOCATION_TRAP = false;                            \
            auto __loc = std::source_location::current();             \
            auto __msg = std::format(                                 \
                "Assertion failed: {}\nLocation: {}:{}\nMessage: {}", \
                #condition,                                           \
                __loc.file_name(),                                    \
                __loc.line(),                                         \
                std::format(__VA_ARGS__)                              \
            );                                                        \
            FRAME_ALLOCATION_TRAP = true;                             \
            FB_INTERNAL_POST_ASSERTION(__msg);                        \
        }                                                             \
    } while (false)

#define FB_ASSERT_HR(hr)                                  \
    do {                                                  \
        if (FAILED(hr)) [[unlikely]] {                    \
            FRAME_ALLOCATION_TRAP = false;                \
            auto __loc = std::source_location::current(); \
            auto __msg = std::format(                     \
                "HRESULT failed: {}\nLocation: {}:{}",    \
                fb::error_detail::hr_message(hr),         \
                __loc.file_name(),                        \
                __loc.line()                              \
            );                                            \
            FRAME_ALLOCATION_TRAP = true;                 \
            FB_INTERNAL_POST_ASSERTION(__msg);            \
        }                                                 \
    } while (false)

#define FB_INTERNAL_POST_ASSERTION(msg) \
    fb::output_debug_string(msg);       \
    fb::output_console_string(msg);     \
    if (IsDebuggerPresent())            \
        __debugbreak();                 \
    FB_FATAL();

} // namespace fb
