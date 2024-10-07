#pragma once

#include "pch.hpp"

//
// Macros.
//

#define FB_PERF_FRAME(index) \
    FrameMark; \
    PIXScopedEvent(PIX_COLOR_DEFAULT, "Frame %zu", index);

#define FB_PERF_FUNC() \
    ZoneScoped;        \
    PIXScopedEvent(PIX_COLOR_DEFAULT, __FUNCTION__);

#define FB_PERF_SCOPE(name) \
    ZoneScopedN(name);      \
    PIXScopedEvent(PIX_COLOR_DEFAULT, name);

#define FB_PERF_VALUE(value) \
    ZoneValue(value);

//
// Tracy.
//

namespace fb {

auto tracy_capture() -> void;

} // namespace fb
