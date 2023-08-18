#include "image.hpp"

namespace fb {

auto Image::load(std::span<const std::byte> image_data) -> Image {
    Image image;
    int dummy;
    auto pixels = stbi_load_from_memory(
        (const stbi_uc*)image_data.data(),
        (int)image_data.size(),
        (int*)&image._width,
        (int*)&image._height,
        &dummy,
        image._channels);
    FB_ASSERT(pixels != nullptr);
    image._pixels.resize(image._width * image._height * image._channels);
    memcpy(image._pixels.data(), pixels, image._pixels.size());
    stbi_image_free(pixels);
    return image;
}

} // namespace fb
