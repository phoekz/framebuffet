#pragma once

#include "../pch.hpp"

namespace fb {

class Image {
public:
    static auto load(std::span<const std::byte> image_data) -> Image;

    auto width() const -> uint32_t { return _width; }
    auto height() const -> uint32_t { return _height; }
    auto size() const -> Uint2 { return Uint2(_width, _height); }
    auto channel_count() const -> uint32_t { return _channel_count; }
    auto row_pitch() const -> uint32_t { return width() * channel_count(); }
    auto slice_pitch() const -> uint32_t { return row_pitch() * height(); }
    auto data() const -> std::span<const std::byte> { return _pixels; }

    template<typename F>
    auto map(F f) -> Image {
        Image image;
        image._width = width();
        image._height = height();
        image._channel_count = channel_count();
        image._pixels = std::move(_pixels);

        for (uint32_t y = 0; y < height(); ++y) {
            for (uint32_t x = 0; x < width(); ++x) {
                const auto i = (y * width() + x) * channel_count();
                f(x,
                  y,
                  image._pixels[i + 0],
                  image._pixels[i + 1],
                  image._pixels[i + 2],
                  image._pixels[i + 3]);
            }
        }

        return image;
    }

private:
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _channel_count = 4;
    std::vector<std::byte> _pixels;
};

} // namespace fb
