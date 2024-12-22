#pragma once

#include "pch.hpp"

//
// Macros.
//

#if defined(USE_PIX)
    #define FB_PERF_PIX_FRAME(index) PIXScopedEvent(PIX_COLOR_DEFAULT, "Frame %zu", index)
    #define FB_PERF_PIX_FUNC() PIXScopedEvent(PIX_COLOR_DEFAULT, __FUNCTION__)
    #define FB_PERF_PIX_SCOPE(name) PIXScopedEvent(PIX_COLOR_DEFAULT, name)
    #define FB_PERF_PIX_BEGIN_EVENT(fmt, ...) PIXBeginEvent(PIX_COLOR_DEFAULT, fmt, __VA_ARGS__)
    #define FB_PERF_PIX_END_EVENT() PIXEndEvent()
    #define FB_PERF_PIX_CMD_BEGIN_EVENT(cmd, fmt, ...) \
        PIXBeginEvent(cmd, PIX_COLOR_DEFAULT, fmt, __VA_ARGS__)
    #define FB_PERF_PIX_CMD_END_EVENT(cmd) PIXEndEvent(cmd)
#else
    #define FB_PERF_PIX_FRAME(index)
    #define FB_PERF_PIX_FUNC()
    #define FB_PERF_PIX_SCOPE(name)
    #define FB_PERF_PIX_BEGIN_EVENT(fmt, ...)
    #define FB_PERF_PIX_END_EVENT()
    #define FB_PERF_PIX_CMD_BEGIN_EVENT(cmd, fmt, ...)
    #define FB_PERF_PIX_CMD_END_EVENT(cmd)
#endif

#define FB_PERF_FRAME(index) \
    FrameMark;               \
    FB_PERF_PIX_FRAME(index);

#define FB_PERF_FUNC() \
    ZoneScoped;        \
    FB_PERF_PIX_FUNC();

#define FB_PERF_SCOPE(name) \
    ZoneScopedN(name);      \
    FB_PERF_PIX_SCOPE(name);

#define FB_PERF_VALUE(value) ZoneValue(value);

//
// Functions.
//

namespace fb {

auto get_time_since_process_start() -> double;
auto tracy_capture() -> void;

} // namespace fb
