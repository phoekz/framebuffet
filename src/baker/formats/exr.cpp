#include "exr.hpp"

namespace fb {

auto ExrImage::load(std::string_view path) -> ExrImage {
    Instant now;
    float* pixels = nullptr;
    int width = 0;
    int height = 0;
    const char* err = nullptr;
    int result = LoadEXRWithLayer(&pixels, &width, &height, path.data(), nullptr, &err);
    FB_ASSERT_MSG(result == TINYEXR_SUCCESS, "{}", err);
    FB_LOG_INFO("Loaded {} ({}x{}) in {}s.", path, width, height, now.elapsed_time());

    ExrImage exr_image;
    exr_image._pixels = pixels;
    exr_image._size = {(uint32_t)width, (uint32_t)height};
    return exr_image;
}

ExrImage::~ExrImage() {
    if (_pixels) {
        FB_LOG_INFO("Freeing EXR image {}.", (void*)_pixels);
        std::free((void*)_pixels);
        _pixels = nullptr;
    }
}

} // namespace fb
