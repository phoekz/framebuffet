#pragma once

#include "../pch.hpp"

namespace fb {

struct Inputs {
    bool mouse_left = false;
    int32_t mouse_x = 0;
    int32_t mouse_y = 0;
    float mouse_wheel_y = 0.0f;
};

class Window final {
    FB_NO_COPY_MOVE(Window);

public:
    struct Desc {
        std::string_view title;
        uint width;
        uint height;
    };

    Window() = default;
    ~Window();

    auto create(const Desc& desc) -> void;
    auto update() -> void;

    auto hwnd() const -> HWND { return _handle; }
    auto inputs() const -> const Inputs& { return _inputs; }

private:
    HWND _handle;
    Inputs _inputs;
};

} // namespace fb
