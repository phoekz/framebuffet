#include "image.hpp"
#include "utils.hpp"

#pragma warning(push)
#pragma warning(disable : 4244)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

namespace fb {

Image::Image(std::span<const std::byte> image_data) {
    // Force all images to be R8G8B8A8_UNORM.
    _format = DXGI_FORMAT_R8G8B8A8_UNORM;
    int channels;
    _channels = 4;
    auto pixels = stbi_load_from_memory(
        (const stbi_uc*)image_data.data(),
        (int)image_data.size(),
        (int*)&_width,
        (int*)&_height,
        &channels,
        _channels);
    FAIL_FAST_IF(pixels == nullptr);
    _pixels.resize(_width * _height * _channels);
    memcpy(_pixels.data(), pixels, _pixels.size());
    stbi_image_free(pixels);
}

}  // namespace fb
