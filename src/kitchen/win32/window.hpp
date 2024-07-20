#pragma once

#include "../pch.hpp"

namespace fb {

struct Button {
    int32_t x = 0;
    int32_t y = 0;
    bool is_down = false;
    bool went_down = false;
    bool went_up = false;
};

struct Inputs {
    Button mouse_left;
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
