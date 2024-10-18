#include "baked.hpp"

namespace fb::baked::raydiance {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 7cb2430fd4012ea4438b2a5ec358208d
    FB_PERF_FUNC();
    _data = read_whole_file("fb_raydiance_assets.bin");
    FB_ASSERT(_data.size() == 154200);
}

auto Assets::cube_mesh() const -> Mesh {
    // vertex_count: 24
    // face_count: 12
    // submesh_count: 1
    return Mesh {
        .transform = float4x4(
            // clang-format off
            // 1.000000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 1.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 1.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1p+0f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x1p+0f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        // hash: d83d3ba89f1591682879b254506b0400
        .vertices = transmuted_span<Vertex>(0, 24),
        // hash: 616e076015f03b0288fd27d32b7bf256
        .indices = transmuted_span<Index>(1152, 36),
        // hash: c2ef228c18663b2f674e7b338753b24d
        .submeshes = transmuted_span<Submesh>(1296, 1),
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    // submesh_count: 1
    return Mesh {
        .transform = float4x4(
            // clang-format off
            // 1.000000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 1.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 1.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1p+0f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x1p+0f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        // hash: 49512cd12fd9ad9fa3a962c2c434f4c4
        .vertices = transmuted_span<Vertex>(1308, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(104268, 12480),
        // hash: 8c65f745bf5c3f03b65a1b71019cb8a2
        .submeshes = transmuted_span<Submesh>(154188, 1),
    };
}

auto Shaders::load() -> void {
    // hash: 99aa06d3014798d86001c324468d497f
    FB_PERF_FUNC();
    _data = read_whole_file("fb_raydiance_shaders.bin");
    FB_ASSERT(_data.size() == 0);
}

#undef texture_data

} // namespace fb::baked::raydiance
