#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <span>

namespace fb {

struct Image {
    static Image load(const std::span<std::byte> image_data);

    inline uint32_t row_pitch() const {
        return width * channels;
    }
    inline uint32_t slice_pitch() const {
        return row_pitch() * height;
    }
    inline const std::byte* data() const {
        return pixels.data();
    }

    uint32_t width;
    uint32_t height;
    uint32_t channels;
    std::vector<std::byte> pixels;
};

}  // namespace fb
