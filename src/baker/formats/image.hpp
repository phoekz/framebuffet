#pragma once

#include <common/common.hpp>

namespace fb {

template<typename T>
concept ImagePixel = std::same_as<T, std::byte> || std::same_as<T, float>;

template<ImagePixel T>
class Image {
public:
    static auto from_image(std::span<const std::byte> src_image) -> Image<T>;
    static auto from_constant(uint width, uint height, const std::array<T, 4>& pixel) -> Image<T>;

    auto width() const -> uint { return _width; }
    auto height() const -> uint { return _height; }
    auto size() const -> uint2 { return uint2(_width, _height); }
    auto channel_count() const -> uint { return _channel_count; }
    auto format() const -> DXGI_FORMAT { return _format; }
    auto row_pitch() const -> uint { return _width * _element_byte_count; }
    auto slice_pitch() const -> uint { return _width * _height * _element_byte_count; }
    auto data() const -> std::span<const std::byte> {
        return std::span((const std::byte*)_data.data(), slice_pitch());
    }

    template<typename F>
    auto map(F f) -> Image {
        Image image = *this;
        T* data = reinterpret_cast<T*>(image._data.data());
        for (uint y = 0; y < image._height; ++y) {
            for (uint x = 0; x < image._width; ++x) {
                const auto i = (y * image._width + x) * image._channel_count;
                f(x, y, data[i + 0], data[i + 1], data[i + 2], data[i + 3]);
            }
        }
        return image;
    }

private:
    uint _width = 0;
    uint _height = 0;
    uint _channel_count = 0;
    uint _element_byte_count = 0;
    DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN;
    std::vector<T> _data;
};

using LdrImage = Image<std::byte>;
using HdrImage = Image<float>;

} // namespace fb
