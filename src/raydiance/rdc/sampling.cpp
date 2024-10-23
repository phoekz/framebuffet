#include "sampling.hpp"

namespace fb {

auto rdc_texture_sample(const fb::baked::Texture& texture, float2 texcoord) -> RgbaFloat {
    // Validation.
    FB_ASSERT(
        texture.format == DXGI_FORMAT_R8G8B8A8_UNORM
        || texture.format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
    );

    // Indexing.
    auto u = std::clamp(texcoord.x, 0.0f, 1.0f);
    const auto v = std::clamp(texcoord.y, 0.0f, 1.0f);
    const auto ix = (uint)(u * (texture.width - 1));
    const auto iy = (uint)(v * (texture.height - 1));
    const auto i = (iy * texture.width + ix) * texture.channel_count;

    // Sample.
    const auto data = texture.datas[0].data;
    const auto texel = data.subspan(i, texture.channel_count);
    RgbaFloat rgba;
    rgba.x = (float)texel[0] / 255.0f;
    rgba.y = (float)texel[1] / 255.0f;
    rgba.z = (float)texel[2] / 255.0f;
    rgba.w = (float)texel[3] / 255.0f;

    // Color space.
    if (texture.format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
        rgba.x = linear_from_srgb(rgba.x);
        rgba.y = linear_from_srgb(rgba.y);
        rgba.z = linear_from_srgb(rgba.z);
    }

    return rgba;
}

} // namespace fb
