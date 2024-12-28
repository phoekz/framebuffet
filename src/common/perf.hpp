#pragma once

#include "pch.hpp"

//
// Macros.
//

#if defined(TRACY_ENABLE)
    #define FB_PERF_TRACY_SET_PROGRAM_NAME(name) TracySetProgramName(name)
    #define FB_PERF_TRACY_FRAME_MARK() FrameMark
    #define FB_PERF_TRACY_ZONE_SCOPED() ZoneScoped
    #define FB_PERF_TRACY_ZONE_SCOPED_N(name) ZoneScopedN(name)
    #define FB_PERF_TRACY_ZONE_VALUE(value) ZoneValue(value)
    #define FB_PERF_TRACY_ZONE_TEXT(name, text) ZoneText(name, text)
    #define FB_PERF_TRACY_ALLOC(ptr, size) TracyAlloc(ptr, size)
    #define FB_PERF_TRACY_FREE(ptr) TracyFree(ptr)
#else
    #define FB_PERF_TRACY_SET_PROGRAM_NAME(name)
    #define FB_PERF_TRACY_FRAME_MARK()
    #define FB_PERF_TRACY_ZONE_SCOPED()
    #define FB_PERF_TRACY_ZONE_SCOPED_N(name)
    #define FB_PERF_TRACY_ZONE_VALUE(value)
    #define FB_PERF_TRACY_ZONE_TEXT(name, text)
    #define FB_PERF_TRACY_ALLOC(ptr, size)
    #define FB_PERF_TRACY_FREE(ptr)
#endif

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

#define FB_PERF_FRAME(index)    \
    FB_PERF_TRACY_FRAME_MARK(); \
    FB_PERF_PIX_FRAME(index);

#define FB_PERF_FUNC()           \
    FB_PERF_TRACY_ZONE_SCOPED(); \
    FB_PERF_PIX_FUNC();

#define FB_PERF_SCOPE(name)            \
    FB_PERF_TRACY_ZONE_SCOPED_N(name); \
    FB_PERF_PIX_SCOPE(name);

#define FB_PERF_VALUE(value) FB_PERF_TRACY_ZONE_VALUE(value);

//
// Functions.
//

namespace fb {

auto get_time_since_process_start() -> double;
auto tracy_capture() -> void;

} // namespace fb
