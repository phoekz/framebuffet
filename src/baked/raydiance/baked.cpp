#include "baked.hpp"

namespace fb::baked::raydiance {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: cd2fb6736ed37e92c641c32a062ecd16
    FB_PERF_FUNC();
    _data = read_whole_file("fb_raydiance_assets.bin");
    FB_ASSERT(_data.size() == 294444);
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
    // vertex_count: 2092
    // face_count: 2862
    // submesh_count: 2
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
        // hash: 4c3f6d2f77bd50dbe8802b12618da099
        .vertices = transmuted_span<Vertex>(154200, 2092),
        // hash: 022559a91fe6658119a12308588e710c
        .indices = transmuted_span<Index>(254616, 8586),
        // hash: 63483ff816dab7a6a3cfefe47ed35bef
        .submeshes = transmuted_span<Submesh>(288960, 2),
    };
}

auto Assets::rounded_cube_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data( 128,    4096,    288984,    4096); // hash: bb15d9fb8c01e5c221dcc0af48dfd89d, width: 32, height: 32
    datas[ 1] = texture_data(  64,    1024,    293080,    1024); // hash: 495ebb7f37bcf8f373906355a2f4ef88, width: 16, height: 16
    datas[ 2] = texture_data(  32,     256,    294104,     256); // hash: fd2547ab7e9275382a8ffbc654278c10, width: 8, height: 8
    datas[ 3] = texture_data(  16,      64,    294360,      64); // hash: 33008f52ca62e5748a12728a8b61b354, width: 4, height: 4
    datas[ 4] = texture_data(   8,      16,    294424,      16); // hash: 2bfe8639ccc553f2b51dc54c3d500160, width: 2, height: 2
    datas[ 5] = texture_data(   4,       4,    294440,       4); // hash: eb0bdd519710d6f6e15ee063eda5ac5e, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 32,
        .height = 32,
        .channel_count = 4,
        .mip_count = 6,
        .datas = datas,
    };
}

auto Assets::rounded_cube_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
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
