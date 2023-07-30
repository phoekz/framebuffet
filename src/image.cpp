#include "image.hpp"
#include "utils.hpp"

#pragma warning(push)
#pragma warning(disable : 4244)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

namespace fb {

Image Image::load(const std::span<std::byte> image_data) {
    Image image;
    int channels;
    image.channels = 4;
    auto pixels = stbi_load_from_memory(
        (const stbi_uc*)image_data.data(),
        (int)image_data.size(),
        (int*)&image.width,
        (int*)&image.height,
        &channels,
        image.channels);
    FAIL_FAST_IF(pixels == nullptr);
    image.pixels.resize(image.width * image.height * image.channels);
    memcpy(image.pixels.data(), pixels, image.pixels.size());
    stbi_image_free(pixels);
    return image;
}

}  // namespace fb
