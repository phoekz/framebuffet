#include "baked.hpp"

namespace fb::baked::raydiance {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 29c37c34b03248980cfb4b4b17397278
    FB_PERF_FUNC();
    _data = read_whole_file("fb_raydiance_assets.bin");
    FB_ASSERT(_data.size() == 310832);
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

auto Assets::rounded_cube_mesh() const -> Mesh {
    // vertex_count: 2088
    // face_count: 2860
    // submesh_count: 1
    return Mesh {
        .transform = float4x4(
            // clang-format off
            // 2.000000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 2.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 2.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1p+1f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1p+1f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x1p+1f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        // hash: a32f5011e497cf1f85dfb48ead9ca401
        .vertices = transmuted_span<Vertex>(154200, 2088),
        // hash: 64edff1faf8cb95f1c8c921871c0398b
        .indices = transmuted_span<Index>(254424, 8580),
        // hash: f0d8ade6012fb22abe1567d2bd1897cf
        .submeshes = transmuted_span<Submesh>(288744, 1),
    };
}

auto Assets::rounded_cube_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(   4,       4,    288756,       4); // hash: 27f693aa771c16015466d1025f82aca2, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1,
        .height = 1,
        .channel_count = 4,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::rounded_cube_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::plane_mesh() const -> Mesh {
    // vertex_count: 4
    // face_count: 2
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
        // hash: fc0bfb437fbe4b7bde660d46d2d9c053
        .vertices = transmuted_span<Vertex>(288760, 4),
        // hash: e61613adcfc67d4d78864c88896b5ba0
        .indices = transmuted_span<Index>(288952, 6),
        // hash: c98593cf018afad07e4bedad79142b75
        .submeshes = transmuted_span<Submesh>(288976, 1),
    };
}

auto Assets::plane_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data( 256,   16384,    288988,   16384); // hash: da2d47e127df9fdc084135968d646891, width: 64, height: 64
    datas[ 1] = texture_data( 128,    4096,    305372,    4096); // hash: 7f20e38d125579b8440ea035c9147374, width: 32, height: 32
    datas[ 2] = texture_data(  64,    1024,    309468,    1024); // hash: a419af177b1b69bb6e22a51392a4e806, width: 16, height: 16
    datas[ 3] = texture_data(  32,     256,    310492,     256); // hash: c2fbbe9ab9467d3f4998ea1f82473e95, width: 8, height: 8
    datas[ 4] = texture_data(  16,      64,    310748,      64); // hash: fa1572b701b6ed624dfe94358604f0eb, width: 4, height: 4
    datas[ 5] = texture_data(   8,      16,    310812,      16); // hash: a77803af8e3526796b1a201bc1f800ba, width: 2, height: 2
    datas[ 6] = texture_data(   4,       4,    310828,       4); // hash: 18a31d7b8588932d35a7129091da92d3, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 64,
        .height = 64,
        .channel_count = 4,
        .mip_count = 7,
        .datas = datas,
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
