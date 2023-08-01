#pragma once

#include <pch.hpp>

namespace fb {

class WindowImpl;

class Window final {
  public:
    struct Desc {
        std::string_view title;
        uint32_t width;
        uint32_t height;
    };

    Window(const Desc& desc);
    ~Window();

    auto hwnd() const -> HWND;

  private:
    std::unique_ptr<WindowImpl> impl;
};

}  // namespace fb
