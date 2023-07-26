#pragma once

#include <cstdint>

namespace fb {

struct Window;
struct WindowHandle;
struct WindowDesc {
    const char* title;
    uint32_t width;
    uint32_t height;
};
Window* window_create(const WindowDesc& desc) noexcept;
void window_destroy(Window* window) noexcept;
WindowHandle* window_handle(Window* window) noexcept;

}  // namespace fb
