#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <span>
#include <dxgiformat.h>

namespace fb {

class Image {
  public:
    Image(std::span<const std::byte> image_data);

    auto format() const -> DXGI_FORMAT { return _format; }
    auto width() const -> uint32_t { return _width; }
    auto height() const -> uint32_t { return _height; }
    auto channels() const -> uint32_t { return _channels; }
    auto row_pitch() const -> uint32_t { return width() * channels(); }
    auto slice_pitch() const -> uint32_t { return row_pitch() * height(); }
    auto data() const -> const std::byte* { return _pixels.data(); }

  private:
    DXGI_FORMAT _format;
    uint32_t _width;
    uint32_t _height;
    uint32_t _channels;
    std::vector<std::byte> _pixels;
};

}  // namespace fb
