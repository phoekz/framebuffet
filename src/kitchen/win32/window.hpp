#pragma once

#include "../pch.hpp"

namespace fb {

class Window final {
    FB_NO_COPY_MOVE(Window);

public:
    struct Desc {
        std::string_view title;
        uint32_t width;
        uint32_t height;
    };

    Window() = default;

    auto create(const Desc& desc) -> void;
    auto hwnd() const -> HWND;

private:
    wil::unique_hwnd _handle;
};

} // namespace fb
