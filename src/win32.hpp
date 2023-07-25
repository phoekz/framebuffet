#pragma once

#include <cstdint>

namespace fb {

typedef void* Window;
struct WindowDesc {
    const char* title;
    uint32_t width;
    uint32_t height;
};
Window window_create(const WindowDesc& desc) noexcept;
void window_destroy(Window window) noexcept;

}  // namespace fb
