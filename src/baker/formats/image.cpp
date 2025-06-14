#include "image.hpp"

#include <stb_image.h>
#include <tinyexr.h>

namespace fb {

static constexpr uint CHANNEL_COUNT = 4;

template<>
auto Image<std::byte>::from_image(Span<const std::byte> src_image) -> Image<std::byte> {
    // Load.
    uint width = 0;
    uint height = 0;
    uint channels_in_file = 0;
    auto src_data = stbi_load_from_memory(
        (const stbi_uc*)src_image.data(),
        (int)src_image.size(),
        (int*)&width,
        (int*)&height,
        (int*)&channels_in_file,
        CHANNEL_COUNT
    );
    FB_ASSERT(src_data != nullptr);

    // Copy.
    std::vector<std::byte> dst_data;
    dst_data.resize(width * height * CHANNEL_COUNT);
    memcpy(dst_data.data(), src_data, dst_data.size());
    stbi_image_free(src_data);

    // Result.
    Image image;
    image._width = width;
    image._height = height;
    image._channel_count = CHANNEL_COUNT;
    image._element_byte_count = CHANNEL_COUNT * sizeof(std::byte);
    image._format = DXGI_FORMAT_R8G8B8A8_UNORM;
    image._data = std::move(dst_data);
    return image;
}

template<>
auto Image<float>::from_image(Span<const std::byte> src_image) -> Image<float> {
    // Load.
    uint width = 0;
    uint height = 0;
    float* src_data = nullptr;
    const char* error_msg = nullptr;
    int result = LoadEXRFromMemory(
        &src_data,
        (int*)&width,
        (int*)&height,
        (const uint8_t*)src_image.data(),
        src_image.size(),
        &error_msg
    );
    FB_ASSERT_MSG(result == TINYEXR_SUCCESS, "{}", error_msg);

    // Copy.
    std::vector<float> dst_data;
    dst_data.resize(width * height * CHANNEL_COUNT);
    const size_t src_byte_count = width * height * CHANNEL_COUNT * sizeof(float);
    memcpy(dst_data.data(), src_data, src_byte_count);
    std::free((void*)src_data);

    // Result.
    Image image;
    image._width = width;
    image._height = height;
    image._channel_count = CHANNEL_COUNT;
    image._element_byte_count = CHANNEL_COUNT * sizeof(float);
    image._format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    image._data = std::move(dst_data);
    return image;
}

template<>
auto Image<std::byte>::from_constant(uint width, uint height, const std::array<std::byte, 4>& pixel)
    -> Image<std::byte> {
    // Copy.
    std::vector<std::byte> dst_data;
    dst_data.resize(width * height * CHANNEL_COUNT);
    for (size_t i = 0; i < dst_data.size(); i += CHANNEL_COUNT) {
        memcpy(dst_data.data() + i, pixel.data(), CHANNEL_COUNT);
    }

    // Result.
    Image image;
    image._width = width;
    image._height = height;
    image._channel_count = CHANNEL_COUNT;
    image._element_byte_count = CHANNEL_COUNT * sizeof(std::byte);
    image._format = DXGI_FORMAT_R8G8B8A8_UNORM;
    image._data = std::move(dst_data);
    return image;
}

template<>
auto Image<float>::from_constant(uint width, uint height, const std::array<float, 4>& pixel)
    -> Image<float> {
    // Copy.
    std::vector<float> dst_data;
    dst_data.resize(width * height * CHANNEL_COUNT);
    for (size_t i = 0; i < dst_data.size(); i += CHANNEL_COUNT) {
        memcpy(dst_data.data() + i, pixel.data(), CHANNEL_COUNT * sizeof(float));
    }

    // Result.
    Image image;
    image._width = width;
    image._height = height;
    image._channel_count = CHANNEL_COUNT;
    image._element_byte_count = CHANNEL_COUNT * sizeof(float);
    image._format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    image._data = std::move(dst_data);
    return image;
}

} // namespace fb
