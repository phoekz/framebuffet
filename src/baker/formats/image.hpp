#pragma once

#include "../pch.hpp"

namespace fb {

class Image {
public:
    static auto load(std::span<const std::byte> image_data) -> Image;

    auto width() const -> uint32_t { return _width; }
    auto height() const -> uint32_t { return _height; }
    auto channels() const -> uint32_t { return _channels; }
    auto row_pitch() const -> uint32_t { return width() * channels(); }
    auto slice_pitch() const -> uint32_t { return row_pitch() * height(); }
    auto data() const -> std::span<const std::byte> { return _pixels; }

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _channels = 4;
    std::vector<std::byte> _pixels;
};

} // namespace fb
