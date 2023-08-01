#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <span>
#include <dxgiformat.h>

namespace fb {

class Image {
  public:
    static auto load(std::span<const std::byte> image_data) -> Image;

    auto format() const -> DXGI_FORMAT { return _format; }
    auto width() const -> uint32_t { return _width; }
    auto height() const -> uint32_t { return _height; }
    auto channels() const -> uint32_t { return _channels; }
    auto row_pitch() const -> uint32_t { return width() * channels(); }
    auto slice_pitch() const -> uint32_t { return row_pitch() * height(); }
    auto data() const -> const std::byte* { return _pixels.data(); }

  private:
    DXGI_FORMAT _format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _channels = 4;
    std::vector<std::byte> _pixels;
};

}  // namespace fb
