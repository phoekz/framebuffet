#pragma once

#include <pch.hpp>
#include "maths.hpp"

namespace fb {

class ExrImage {
  public:
    static auto load(std::string_view path) -> ExrImage;

    ~ExrImage();
    ExrImage(ExrImage&& o) { *this = std::move(o); }
    ExrImage& operator=(ExrImage&& o) {
        _pixels = std::exchange(o._pixels, nullptr);
        _size = std::exchange(o._size, Uint2 {0, 0});
        return *this;
    }

    auto data() const -> const float* { return _pixels; }
    auto size() const -> Uint2 { return _size; }
    auto width() const -> uint32_t { return _size.x; }
    auto height() const -> uint32_t { return _size.y; }
    auto format() const -> DXGI_FORMAT { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
    auto row_pitch() const -> uint32_t { return width() * sizeof(float) * 4; }
    auto slice_pitch() const -> uint32_t { return row_pitch() * height(); }

    ExrImage(const ExrImage&) = delete;
    ExrImage& operator=(const ExrImage&) = delete;

  private:
    ExrImage() {}

    const float* _pixels = nullptr;
    Uint2 _size = {};
};

}  // namespace fb