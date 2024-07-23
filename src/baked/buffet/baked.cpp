#include "baked.hpp"

namespace fb::baked::buffet {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

auto Assets::load() -> void {
    // hash: 22d7e09e509d62774a2bcd476ad75389
    ZoneScoped;
    _data = read_whole_file("fb_buffet_assets.bin");
    FB_ASSERT(_data.size() == 189046280);
}

auto Assets::heatmap_magma_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(1024,    1024,         0,    1024); // hash: 33e2d2ee5bc875a573dd90c6f664a91c, width: 256, height: 1
    datas[ 1] = texture_data( 512,     512,      1024,     512); // hash: 86cc43959a92ff04cedd3d3aa312eee8, width: 128, height: 1
    datas[ 2] = texture_data( 256,     256,      1536,     256); // hash: 734e2c3abf0ea59042a7a406af85f071, width: 64, height: 1
    datas[ 3] = texture_data( 128,     128,      1792,     128); // hash: 25c341c38486da6ac6ba118d8025af92, width: 32, height: 1
    datas[ 4] = texture_data(  64,      64,      1920,      64); // hash: 37ef2403766d2fac303b4bea97adf132, width: 16, height: 1
    datas[ 5] = texture_data(  32,      32,      1984,      32); // hash: bd143bc176b967ced1d48e4f8a4f2cb9, width: 8, height: 1
    datas[ 6] = texture_data(  16,      16,      2016,      16); // hash: 6ffa2da24c9dee7344309befeccb2190, width: 4, height: 1
    datas[ 7] = texture_data(   8,       8,      2032,       8); // hash: 5ebdc1722d8abe18ccc4c3a49ff06d72, width: 2, height: 1
    datas[ 8] = texture_data(   4,       4,      2040,       4); // hash: 448ab11a285422615f166b43c7d70bfb, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .mip_count = 9,
        .datas = datas,
    };
}

auto Assets::heatmap_viridis_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(1024,    1024,      2044,    1024); // hash: 68bceaeffeb954c3304af1cdb4a1e5fc, width: 256, height: 1
    datas[ 1] = texture_data( 512,     512,      3068,     512); // hash: 325545a90af39f65504becc272d17d1b, width: 128, height: 1
    datas[ 2] = texture_data( 256,     256,      3580,     256); // hash: 1872652d1b188d98960797a861a05c57, width: 64, height: 1
    datas[ 3] = texture_data( 128,     128,      3836,     128); // hash: 0defd6e02d84d1b2acf60921732d378e, width: 32, height: 1
    datas[ 4] = texture_data(  64,      64,      3964,      64); // hash: 92294ed3ea02ed1f4319cd20bf3f71c6, width: 16, height: 1
    datas[ 5] = texture_data(  32,      32,      4028,      32); // hash: 15fcf5da43414ee9b17fc9d98ce88bb0, width: 8, height: 1
    datas[ 6] = texture_data(  16,      16,      4060,      16); // hash: 844ea3b742c5f7a9dd087f275be89521, width: 4, height: 1
    datas[ 7] = texture_data(   8,       8,      4076,       8); // hash: 40a6faa288fcc089d080e840b745b320, width: 2, height: 1
    datas[ 8] = texture_data(   4,       4,      4084,       4); // hash: 7a1c709b7c6639090826f10a8cfc7dab, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .mip_count = 9,
        .datas = datas,
    };
}

auto Assets::sci_fi_case_mesh() const -> Mesh {
    // vertex_count: 2025
    // face_count: 1767
    // submesh_count: 3
    return Mesh {
        .transform = float4x4(
            // clang-format off
            // 1.000000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, -1.000000f, 0.000000f
            // 0.000000f, 1.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1p+0f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1p-52f, -0x1p+0f, 0x0p+0f,
            0x0p+0f, 0x1p+0f, 0x1p-52f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        // hash: 576ddd3553b833a1778d067f0cfc0f20
        .vertices = transmuted_span<Vertex>(4088, 2025),
        // hash: b4d34f8bbc09c51115542a8b94aec26a
        .indices = transmuted_span<Index>(101288, 5301),
        // hash: ce814971953cf8e9c2bb3870ebaf64ad
        .submeshes = transmuted_span<Submesh>(122492, 3),
    };
}

auto Assets::sci_fi_case_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,    122528, 4194304); // hash: 1c0cad906e975f6b2b0f9d24de7c4926, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,   4316832, 1048576); // hash: 6bebe2067d611dfae3145d5f69099e02, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,   5365408,  262144); // hash: 5a31a9aea69b9572403af8c398ff1bb1, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,   5627552,   65536); // hash: a7885b770b2e31e7cc2b44a128f01c11, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,   5693088,   16384); // hash: 57315ba87386a5f4d7792ac48fafbcc2, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,   5709472,    4096); // hash: a58e3210388634d7155f2901e7501649, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,   5713568,    1024); // hash: 148fb23c5ce094c10a5185b87ded78f6, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,   5714592,     256); // hash: 2b29afab47c66cc6470d9fd805320d9c, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,   5714848,      64); // hash: 618df3e3abd8ce160684dd0e4549ce74, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,   5714912,      16); // hash: 774155eac0ec01f795a1255477bebbb4, width: 2, height: 2
    datas[10] = texture_data(   4,       4,   5714928,       4); // hash: f27ac865f3272414b83c54340f946f64, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::sci_fi_case_normal_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,   5714932, 4194304); // hash: d5c691d45d7f61290eb8b12ed954e8f1, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,   9909236, 1048576); // hash: 45737997d8e5603f7261972b99fbaccc, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  10957812,  262144); // hash: c0351b94f1099be07060ed28304164a7, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  11219956,   65536); // hash: e055a207782a7cbb779e6399a59d5adb, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  11285492,   16384); // hash: cbdd29d27d52e586df02e8073b6d017b, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  11301876,    4096); // hash: dfe0b70bf322d90fb2bff80c5ab1d8c9, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  11305972,    1024); // hash: b3aa202e184073b05965c82d4e60ec78, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  11306996,     256); // hash: a592dfd5855b6ee57b42d9edfa7f2833, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  11307252,      64); // hash: c3439bbdf4acc22b41baf1a96203e800, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  11307316,      16); // hash: 17e7ca5da0e3c69176067d4cf9dff17a, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  11307332,       4); // hash: f334417f8373d6f0ef2e2f2651e34647, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::sci_fi_case_metallic_roughness_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,  11307336, 4194304); // hash: ed04eb3dfe14725c0ad06ee90e903d7b, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  15501640, 1048576); // hash: f88d97a61574036a8d8003bc83c0509c, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  16550216,  262144); // hash: b8a656249f0ad55da668079c25e762a7, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  16812360,   65536); // hash: dab5fa896417c0d46d91b48bbf51b91d, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  16877896,   16384); // hash: ae6b271b6cedc3117259743503987475, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  16894280,    4096); // hash: 803f12a9da7dc4117b935a6570eb6e62, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  16898376,    1024); // hash: 2de9c42bfe1107d80ba68be92c4133dd, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  16899400,     256); // hash: d8a8fba03904f3e9c7123cb6b038fece, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  16899656,      64); // hash: 5361feba23fce4703e3c634048c080d7, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  16899720,      16); // hash: f2b6d357693a6c96c8686a98bd80974b, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  16899736,       4); // hash: a67c136df57f927e6e07eab72b4f022b, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::sci_fi_case_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::metal_plane_mesh() const -> Mesh {
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
        // hash: 7513db2c165692b22a3bb4374738dab7
        .vertices = transmuted_span<Vertex>(16899740, 4),
        // hash: 96880b352a2cd08ebe7a559eac217606
        .indices = transmuted_span<Index>(16899932, 6),
        // hash: c98593cf018afad07e4bedad79142b75
        .submeshes = transmuted_span<Submesh>(16899956, 1),
    };
}

auto Assets::metal_plane_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,  16899968, 4194304); // hash: b01e0981487f9386f14d7e555623c2ba, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  21094272, 1048576); // hash: 571945c38522b7a2ddf3b05507efe984, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  22142848,  262144); // hash: 872860b3b5fcef1b4c5899d1961e67a5, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  22404992,   65536); // hash: 079ffd2caa9c297f2b04aaab46f01bcb, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  22470528,   16384); // hash: 257a6272975283c2ba3982e0f48f3871, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  22486912,    4096); // hash: f3e2a564cd47f149462ab82d010ac6fc, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  22491008,    1024); // hash: 043e70e3d266bc8fa1af4ad7203787c9, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  22492032,     256); // hash: 144abce73f4bb6a7748614fffa83c5cd, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  22492288,      64); // hash: de1352b64f46a4757900083a9975b5a4, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  22492352,      16); // hash: 6d587cac4b377cdce6b029d7b6aa9ed6, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  22492368,       4); // hash: 275847d06c6d2099829491e6f3f23a61, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::metal_plane_normal_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,  22492372, 4194304); // hash: 2c3220bc625827e8a3a07df006fd2577, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  26686676, 1048576); // hash: c3c55beeb71deeb83d1f282bdf57ec76, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  27735252,  262144); // hash: 78bb93d9adee26e3e1dc1886dae718ae, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  27997396,   65536); // hash: 861b5f55bc01af94eccb54fadede533a, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  28062932,   16384); // hash: c6241fb22bbdb204055a36c59e19eaa7, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  28079316,    4096); // hash: 96a28968bbeb40fd031cca35a218225c, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  28083412,    1024); // hash: c588a82ede6113e9a066f3ae10347b68, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  28084436,     256); // hash: 23f1fc279c052f32433e694e9dcf34d8, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  28084692,      64); // hash: cfc2177f15b55d6e9cd459a7ec5de27c, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  28084756,      16); // hash: 56c2be73c379af90ab87d34bc436619b, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  28084772,       4); // hash: 01862aadb9ce1ab37fe3803a108ced53, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::metal_plane_metallic_roughness_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,  28084776, 4194304); // hash: a0a8987101c3763d88cded178f004edc, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  32279080, 1048576); // hash: 433096216c0dff0d4b79e43f8ac9a5da, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  33327656,  262144); // hash: d663dd53772552e20d8c124109041e42, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  33589800,   65536); // hash: 52a2e110668972c369b81ca2f7d03732, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  33655336,   16384); // hash: 8df31f4575f5e3b5bdeb96cc013a2505, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  33671720,    4096); // hash: f06fd168844ac517f257d6cde21e3c3e, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  33675816,    1024); // hash: fd4f859331efb591100afda7bd764091, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  33676840,     256); // hash: e5c5824300a76bb01e0e389a992d2eb8, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  33677096,      64); // hash: 0f88755d2cf041ee01b24e89928c89f4, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  33677160,      16); // hash: f77b6b506c3f744884e3d267355042be, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  33677176,       4); // hash: eb09f6f496892ae3a96abfd9da49d00b, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::metal_plane_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    // vertex_count: 725
    // face_count: 678
    // submesh_count: 1
    return Mesh {
        .transform = float4x4(
            // clang-format off
            // 1.000000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, -1.000000f, 0.000000f
            // 0.000000f, 1.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1p+0f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1p-52f, -0x1p+0f, 0x0p+0f,
            0x0p+0f, 0x1p+0f, 0x1p-52f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        // hash: cee14e1c21b8adac5f904691e307303a
        .vertices = transmuted_span<Vertex>(33677180, 725),
        // hash: 2e21503f47bdce31841a1d7890743c68
        .indices = transmuted_span<Index>(33711980, 2034),
        // hash: f602854e3b4643fdfd28d39aba429ae6
        .submeshes = transmuted_span<Submesh>(33720116, 1),
    };
}

auto Assets::coconut_tree_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(2048, 1048576,  33720128, 1048576); // hash: 61b50bca3a05bbee4e3297d007f5fc25, width: 512, height: 512
    datas[ 1] = texture_data(1024,  262144,  34768704,  262144); // hash: 0edae200d9d108428bd9b503d8169fbf, width: 256, height: 256
    datas[ 2] = texture_data( 512,   65536,  35030848,   65536); // hash: bcf7be7062f52f12be92c226bfaf7e84, width: 128, height: 128
    datas[ 3] = texture_data( 256,   16384,  35096384,   16384); // hash: 7c277a922339cb6f8cee9b7d4fb0d456, width: 64, height: 64
    datas[ 4] = texture_data( 128,    4096,  35112768,    4096); // hash: 2251821b2955fa891380f4eac15bcd6c, width: 32, height: 32
    datas[ 5] = texture_data(  64,    1024,  35116864,    1024); // hash: 5b943a736dfa82072e5cb6affe649865, width: 16, height: 16
    datas[ 6] = texture_data(  32,     256,  35117888,     256); // hash: c842b2708e4811562e58fa88d52283be, width: 8, height: 8
    datas[ 7] = texture_data(  16,      64,  35118144,      64); // hash: 4b46d1e4178e56188b4c00c7fc23cb42, width: 4, height: 4
    datas[ 8] = texture_data(   8,      16,  35118208,      16); // hash: 570f797ebb674c8e914531a49491f97f, width: 2, height: 2
    datas[ 9] = texture_data(   4,       4,  35118224,       4); // hash: 68d5e1ca0194c1023749ac2880a275c9, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .mip_count = 10,
        .datas = datas,
    };
}

auto Assets::coconut_tree_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::sand_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data( 256,   16384,  35118228,   16384); // hash: cd9c6abea5eb3cc9b5846ab373f9d5d8, width: 64, height: 64
    datas[ 1] = texture_data( 128,    4096,  35134612,    4096); // hash: da49ee29170ccee71bde3a122fb1ae75, width: 32, height: 32
    datas[ 2] = texture_data(  64,    1024,  35138708,    1024); // hash: 63e53ce8aff7a307f0c1097ccf5d80c5, width: 16, height: 16
    datas[ 3] = texture_data(  32,     256,  35139732,     256); // hash: 00f8f27693c1fe15fbdd5456945dfa9f, width: 8, height: 8
    datas[ 4] = texture_data(  16,      64,  35139988,      64); // hash: 5743429ba6b07a05fffee7cf591b7cc5, width: 4, height: 4
    datas[ 5] = texture_data(   8,      16,  35140052,      16); // hash: c32f5c05e144ea6a196dd524d8dae576, width: 2, height: 2
    datas[ 6] = texture_data(   4,       4,  35140068,       4); // hash: 8005525e757dc43ff5d1826186e49ea5, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .mip_count = 7,
        .datas = datas,
    };
}

auto Assets::sand_mesh() const -> Mesh {
    // vertex_count: 5766
    // face_count: 1922
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
        // hash: 7fa771f6b82914ced15ac4b1e422d018
        .vertices = transmuted_span<Vertex>(35140072, 5766),
        // hash: 6b2f255bcca5115fc052b8dc1591ccb3
        .indices = transmuted_span<Index>(35416840, 5766),
        // hash: f01db1e83ea8676f9a22f03af7b4df58
        .submeshes = transmuted_span<Submesh>(35439904, 1),
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    // vertex_count: 2430
    // face_count: 1034
    // submesh_count: 1
    return AnimationMesh {
        .transform = float4x4(
            // clang-format off
            // 0.014462f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, -0.014462f, 0.000000f
            // 0.000000f, 0.014462f, 0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1.d9e4fap-7f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, 0x1.d9e4fap-59f, -0x1.d9e4fap-7f, 0x0p+0f,
            0x0p+0f, 0x1.d9e4fap-7f, 0x1.d9e4fap-59f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        // hash: e65e9d279ecf4a2eac69f51658a72576
        .skinning_vertices = transmuted_span<SkinningVertex>(35439916, 2430),
        // hash: fd12995a38e89146d53c7edd4f6cd3d0
        .indices = transmuted_span<Index>(35634316, 3102),
        // hash: 15920e05403f9ada661b67ea27d6e70b
        .submeshes = transmuted_span<Submesh>(35646724, 1),
        // hash: 23f38d0ed7e625efed1c186988256717
        .joint_nodes = transmuted_span<uint>(35646736, 63),
        // hash: ee0ff01354a210fae3930c7992929c24
        .joint_inverse_binds = transmuted_span<float4x4>(35646988, 63),
        // hash: 0a63565a01a515156d594dabc708fa38
        .node_parents = transmuted_span<uint>(35651020, 72),
        // hash: bed19f4b36795de48c26b075b6fafe29
        .node_channels = transmuted_span<AnimationChannel>(35651308, 72),
        // hash: 42b8cc9442f01c71bf8fb0a746312d3c
        .node_channels_times_t = transmuted_span<float>(35654764, 2256),
        // hash: 1fba1ded90e724aae024c3cfd2725791
        .node_channels_times_r = transmuted_span<float>(35663788, 2274),
        // hash: 6ed736313b4d8a3257bbe0707aa6e7a6
        .node_channels_times_s = transmuted_span<float>(35672884, 369),
        // hash: 753a98c2f50fe96360233f10eadabd37
        .node_channels_values_t = transmuted_span<float3>(35674360, 2256),
        // hash: 6967d1f12a8511fd7375db930f59a2ca
        .node_channels_values_r = transmuted_span<Quaternion>(35701432, 2274),
        // hash: 2b1523eab4b57d702b9326dcc49af366
        .node_channels_values_s = transmuted_span<float3>(35737816, 369),
    };
}

auto Assets::raccoon_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(  16,      64,  35742244,      64); // hash: f46411895f74b52626f359cce1525b8e, width: 4, height: 4
    datas[ 1] = texture_data(   8,      16,  35742308,      16); // hash: 937e68bc91d5794588f960b38a352b2d, width: 2, height: 2
    datas[ 2] = texture_data(   4,       4,  35742324,       4); // hash: 44cbb8d6aae19256febe1555bfc8a1a1, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 4,
        .height = 4,
        .mip_count = 3,
        .datas = datas,
    };
}

auto Assets::raccoon_metallic_roughness_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(  16,      64,  35742328,      64); // hash: 22771420d888fb100e7450610145f45d, width: 4, height: 4
    datas[ 1] = texture_data(   8,      16,  35742392,      16); // hash: 0eea1d73e7ab0514131b39b1aad79493, width: 2, height: 2
    datas[ 2] = texture_data(   4,       4,  35742408,       4); // hash: 44ff0e0fb6c73dbbd0b61d69281ee072, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 4,
        .height = 4,
        .mip_count = 3,
        .datas = datas,
    };
}

auto Assets::raccoon_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::mixamo_run_female_animation_mesh() const -> AnimationMesh {
    // vertex_count: 28374
    // face_count: 49112
    // submesh_count: 2
    return AnimationMesh {
        .transform = float4x4(
            // clang-format off
            // 0.010000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, -0.000000f, 0.010000f, 0.000000f
            // 0.000000f, -0.010000f, -0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1.47ae14p-7f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, -0x1.47ae14p-30f, 0x1.47ae16p-7f, 0x0p+0f,
            0x0p+0f, -0x1.47ae16p-7f, -0x1.47ae14p-30f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        .node_count = 68,
        .joint_count = 65,
        .duration = 0.5833334f,
        // hash: 418753130b12d972c6b059cf3574ba05
        .skinning_vertices = transmuted_span<SkinningVertex>(35742412, 28374),
        // hash: a8011b7a24a42ccfe41a71da6bfb4420
        .indices = transmuted_span<Index>(38012332, 147336),
        // hash: 5c8f60efa6c0d8fe0660d0726be8c3dc
        .submeshes = transmuted_span<Submesh>(38601676, 2),
        // hash: 88f17a68c86dca0723a0e8beb8c33c42
        .joint_nodes = transmuted_span<uint>(38601700, 65),
        // hash: be26d2e13165af64c51716bd6803cb8a
        .joint_inverse_binds = transmuted_span<float4x4>(38601960, 65),
        // hash: 84cb50e7cf3334aeab71f485869a54f2
        .node_parents = transmuted_span<uint>(38606120, 68),
        // hash: ff2c66670bf7cd6ab64dc1b674a32588
        .node_channels = transmuted_span<AnimationChannel>(38606392, 68),
        // hash: 8d4cefd6b34018aa65aca9c6a8d0bde3
        .node_channels_times_t = transmuted_span<float>(38609656, 164),
        // hash: b1bb3ef9ff6638c88f594ab115caf264
        .node_channels_times_r = transmuted_span<float>(38610312, 878),
        // hash: 2be0afe218e322612b3848cb4820c1e4
        .node_channels_times_s = transmuted_span<float>(38613824, 130),
        // hash: f89423b5d9717d813e3e2349236f4c47
        .node_channels_values_t = transmuted_span<float3>(38614344, 164),
        // hash: 8570f2d57e7b63c80933e9f98347df13
        .node_channels_values_r = transmuted_span<Quaternion>(38616312, 878),
        // hash: 0809bf1ba49e1543f903b075bf5eec3a
        .node_channels_values_s = transmuted_span<float3>(38630360, 130),
    };
}

auto Assets::mixamo_run_female_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(   4,       4,  38631920,       4); // hash: 475aa22946a0428d115c902d375c1ad7, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1,
        .height = 1,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::mixamo_run_female_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::mixamo_run_male_animation_mesh() const -> AnimationMesh {
    // vertex_count: 35434
    // face_count: 55320
    // submesh_count: 2
    return AnimationMesh {
        .transform = float4x4(
            // clang-format off
            // 0.010000f, 0.000000f, 0.000000f, 0.000000f
            // 0.000000f, -0.000000f, 0.010000f, 0.000000f
            // 0.000000f, -0.010000f, -0.000000f, 0.000000f
            // 0.000000f, 0.000000f, 0.000000f, 1.000000f
            0x1.47ae14p-7f, 0x0p+0f, 0x0p+0f, 0x0p+0f,
            0x0p+0f, -0x1.47ae14p-30f, 0x1.47ae16p-7f, 0x0p+0f,
            0x0p+0f, -0x1.47ae16p-7f, -0x1.47ae14p-30f, 0x0p+0f,
            0x0p+0f, 0x0p+0f, 0x0p+0f, 0x1p+0f
            // clang-format on
        ),
        .node_count = 68,
        .joint_count = 65,
        .duration = 0.6333333f,
        // hash: cebce5c9d87384b344c65c6e66d24971
        .skinning_vertices = transmuted_span<SkinningVertex>(38631924, 35434),
        // hash: 6919b0f9c9ff8a8a5f2dd8cc80c6c83f
        .indices = transmuted_span<Index>(41466644, 165960),
        // hash: 704119a318081a7a547d18a794387893
        .submeshes = transmuted_span<Submesh>(42130484, 2),
        // hash: 88f17a68c86dca0723a0e8beb8c33c42
        .joint_nodes = transmuted_span<uint>(42130508, 65),
        // hash: 889086f83c8991564ccc303d9c0819f0
        .joint_inverse_binds = transmuted_span<float4x4>(42130768, 65),
        // hash: 84cb50e7cf3334aeab71f485869a54f2
        .node_parents = transmuted_span<uint>(42134928, 68),
        // hash: c001881a9c252172959d66ee25efe058
        .node_channels = transmuted_span<AnimationChannel>(42135200, 68),
        // hash: 4df3cc0a60a1616f71845c4ffce8f4f8
        .node_channels_times_t = transmuted_span<float>(42138464, 167),
        // hash: e2a3d2030cd5d93652cb8674b6624639
        .node_channels_times_r = transmuted_span<float>(42139132, 2054),
        // hash: e13d3114734eb11c689d92399e24c301
        .node_channels_times_s = transmuted_span<float>(42147348, 130),
        // hash: de369ac4aa619aa335dd96afb1a419ac
        .node_channels_values_t = transmuted_span<float3>(42147868, 167),
        // hash: bd7a534dda40aa02182d6fc5746dd0e7
        .node_channels_values_r = transmuted_span<Quaternion>(42149872, 2054),
        // hash: ab35824f64d90616a3b8911e79565008
        .node_channels_values_s = transmuted_span<float3>(42182736, 130),
    };
}

auto Assets::mixamo_run_male_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(   4,       4,  42184296,       4); // hash: f2ed0c21de794f794cbcbd42744aab82, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1,
        .height = 1,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::mixamo_run_male_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
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
        .vertices = transmuted_span<Vertex>(42184300, 24),
        // hash: 616e076015f03b0288fd27d32b7bf256
        .indices = transmuted_span<Index>(42185452, 36),
        // hash: c2ef228c18663b2f674e7b338753b24d
        .submeshes = transmuted_span<Submesh>(42185596, 1),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
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
        // hash: 952f25941581c525b7890e0284fd3b52
        .vertices = transmuted_span<Vertex>(42185608, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(42186760, 36),
        // hash: c2ef228c18663b2f674e7b338753b24d
        .submeshes = transmuted_span<Submesh>(42186904, 1),
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
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(42186916, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(42289876, 12480),
        // hash: 8c65f745bf5c3f03b65a1b71019cb8a2
        .submeshes = transmuted_span<Submesh>(42339796, 1),
    };
}

auto Assets::winter_evening_lut() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(2048, 1048576,  42339808, 1048576); // hash: 7f7048fcf260a2aa1eae75b62340d74d, width: 512, height: 512
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R16G16_FLOAT,
        .width = 512,
        .height = 512,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::winter_evening_irr() const -> CubeTexture {
    decltype(CubeTexture::datas) datas = {};
    // clang-format off
    datas[0][ 0] = texture_data(1024,  131072,  43388384,  131072); // hash: f1fca1ec93aaac5c8692900b209366a4, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072,  43519456,  131072); // hash: ebdcd9687791a33db4101d416f5e246c, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072,  43650528,  131072); // hash: 9747b535e689e870395045429a78a958, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072,  43781600,  131072); // hash: a392c47563dd2d8f529d6016031ce565, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072,  43912672,  131072); // hash: e70d52519c582ded8307aecbc69261c5, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072,  44043744,  131072); // hash: 06d3e956fa5a867dfb0e6e272bfcc87f, width: 128, height: 128
    // clang-format on
    return CubeTexture {
        .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
        .width = 128,
        .height = 128,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::winter_evening_rad() const -> CubeTexture {
    decltype(CubeTexture::datas) datas = {};
    // clang-format off
    datas[0][ 0] = texture_data(8192, 8388608,  44174816, 8388608); // hash: e70fec4202154d74b817ce87548a491d, width: 1024, height: 1024
    datas[0][ 1] = texture_data(4096, 2097152,  52563424, 2097152); // hash: 85fc929ac43a8d3c7caa5685b297bde8, width: 512, height: 512
    datas[0][ 2] = texture_data(2048,  524288,  54660576,  524288); // hash: 271f1d9f9ae40adfd6ee286fd23d39f9, width: 256, height: 256
    datas[0][ 3] = texture_data(1024,  131072,  55184864,  131072); // hash: dd2419288db4af6d0254f8d7c6822db0, width: 128, height: 128
    datas[0][ 4] = texture_data( 512,   32768,  55315936,   32768); // hash: b7ce6c5e2b8a2132421a1c608e81ba76, width: 64, height: 64
    datas[0][ 5] = texture_data( 256,    8192,  55348704,    8192); // hash: 972ee92b49ddbd3d926c06d10509b884, width: 32, height: 32
    datas[0][ 6] = texture_data( 128,    2048,  55356896,    2048); // hash: d76513ed7aac113939ce51b47e4e2141, width: 16, height: 16
    datas[0][ 7] = texture_data(  64,     512,  55358944,     512); // hash: 7a7ae37458be6407af077e0109f9f2cc, width: 8, height: 8
    datas[0][ 8] = texture_data(  32,     128,  55359456,     128); // hash: 53b80a4f80cd4f35d4eac8a528355cdc, width: 4, height: 4
    datas[0][ 9] = texture_data(  16,      32,  55359584,      32); // hash: 6ea38dfc8963ee6c09ea67573b990ad0, width: 2, height: 2
    datas[0][10] = texture_data(   8,       8,  55359616,       8); // hash: b6faa2f429d0cf1ecf90387a19007e2b, width: 1, height: 1
    datas[1][ 0] = texture_data(8192, 8388608,  55359624, 8388608); // hash: dbf0ebc6a6c592cae9ad8e3a3f2568aa, width: 1024, height: 1024
    datas[1][ 1] = texture_data(4096, 2097152,  63748232, 2097152); // hash: c56c4e38c4b497e7d2896d0022f7e830, width: 512, height: 512
    datas[1][ 2] = texture_data(2048,  524288,  65845384,  524288); // hash: 284c5bf396722b2c924130675a6259d3, width: 256, height: 256
    datas[1][ 3] = texture_data(1024,  131072,  66369672,  131072); // hash: 7b46ba89a5ba62fce63a10e903333ba0, width: 128, height: 128
    datas[1][ 4] = texture_data( 512,   32768,  66500744,   32768); // hash: 8e136d5fc0581d72e060c750ab7fc58f, width: 64, height: 64
    datas[1][ 5] = texture_data( 256,    8192,  66533512,    8192); // hash: 086133860ad9c48e8524f70cb010ca4d, width: 32, height: 32
    datas[1][ 6] = texture_data( 128,    2048,  66541704,    2048); // hash: 9002e8a826bfee8cc8ef4e8a986c32c1, width: 16, height: 16
    datas[1][ 7] = texture_data(  64,     512,  66543752,     512); // hash: f16b5c6316e9a8e819591d2bfeeced15, width: 8, height: 8
    datas[1][ 8] = texture_data(  32,     128,  66544264,     128); // hash: fe94bf054fd0fa5914e85a3fd840fe83, width: 4, height: 4
    datas[1][ 9] = texture_data(  16,      32,  66544392,      32); // hash: 7c810dac33ceaf7725a8b6a3b4a6b761, width: 2, height: 2
    datas[1][10] = texture_data(   8,       8,  66544424,       8); // hash: 95ace211749b7db4373ffb99761b93a6, width: 1, height: 1
    datas[2][ 0] = texture_data(8192, 8388608,  66544432, 8388608); // hash: 60fc398a716fe918ea5179abdbed5da6, width: 1024, height: 1024
    datas[2][ 1] = texture_data(4096, 2097152,  74933040, 2097152); // hash: 6b56e3c11a6f4266150518a3b87f34c9, width: 512, height: 512
    datas[2][ 2] = texture_data(2048,  524288,  77030192,  524288); // hash: 4194387a266567bc073234a28488ab54, width: 256, height: 256
    datas[2][ 3] = texture_data(1024,  131072,  77554480,  131072); // hash: dda04d5b17e662698ce7d1f09929f5fb, width: 128, height: 128
    datas[2][ 4] = texture_data( 512,   32768,  77685552,   32768); // hash: e36f842c91fd700c1e324c56690d3d05, width: 64, height: 64
    datas[2][ 5] = texture_data( 256,    8192,  77718320,    8192); // hash: 5636f81a9c40ddbd37ad4c2be787db58, width: 32, height: 32
    datas[2][ 6] = texture_data( 128,    2048,  77726512,    2048); // hash: ad241b34b4124b18fb8da354e42cf306, width: 16, height: 16
    datas[2][ 7] = texture_data(  64,     512,  77728560,     512); // hash: 89495bdab42a8777f81c94e53033194c, width: 8, height: 8
    datas[2][ 8] = texture_data(  32,     128,  77729072,     128); // hash: e963c240d9a5dbc71e641bbc8fb40aed, width: 4, height: 4
    datas[2][ 9] = texture_data(  16,      32,  77729200,      32); // hash: 28ba0932c6bfe03072dc26e152d9aff9, width: 2, height: 2
    datas[2][10] = texture_data(   8,       8,  77729232,       8); // hash: e49d8ddf01bacca3e5c7a600bb6c2dcc, width: 1, height: 1
    datas[3][ 0] = texture_data(8192, 8388608,  77729240, 8388608); // hash: b6abe08e32d3e208681502545ec9a502, width: 1024, height: 1024
    datas[3][ 1] = texture_data(4096, 2097152,  86117848, 2097152); // hash: e6f295667f451f1dcd03c57c771a50f8, width: 512, height: 512
    datas[3][ 2] = texture_data(2048,  524288,  88215000,  524288); // hash: 9ba2bd3e24194414485c0ebc3e458501, width: 256, height: 256
    datas[3][ 3] = texture_data(1024,  131072,  88739288,  131072); // hash: 29ebe33d875685675874819ee8a818ca, width: 128, height: 128
    datas[3][ 4] = texture_data( 512,   32768,  88870360,   32768); // hash: 708965672b0d4e23bd2a05d474b625a3, width: 64, height: 64
    datas[3][ 5] = texture_data( 256,    8192,  88903128,    8192); // hash: 5f6d7d4ca99629b1c15e7ef499559efd, width: 32, height: 32
    datas[3][ 6] = texture_data( 128,    2048,  88911320,    2048); // hash: 98c2cba26a372c4e766eb30171b1dd8b, width: 16, height: 16
    datas[3][ 7] = texture_data(  64,     512,  88913368,     512); // hash: 0f469f10c756deff3bc3d6738410532b, width: 8, height: 8
    datas[3][ 8] = texture_data(  32,     128,  88913880,     128); // hash: b60a19c3b4ffcc58ea798a0e15118275, width: 4, height: 4
    datas[3][ 9] = texture_data(  16,      32,  88914008,      32); // hash: 713724c115e1bd4901def7feabb124c7, width: 2, height: 2
    datas[3][10] = texture_data(   8,       8,  88914040,       8); // hash: b04fe0e7106782ae1102d31cd21ed5b6, width: 1, height: 1
    datas[4][ 0] = texture_data(8192, 8388608,  88914048, 8388608); // hash: b33270086b73324a5eaed477b1b03a6b, width: 1024, height: 1024
    datas[4][ 1] = texture_data(4096, 2097152,  97302656, 2097152); // hash: eb71724ac6de7c410c611c34e93379d8, width: 512, height: 512
    datas[4][ 2] = texture_data(2048,  524288,  99399808,  524288); // hash: 3496fe103c6019cb8cb6fdb9e085af48, width: 256, height: 256
    datas[4][ 3] = texture_data(1024,  131072,  99924096,  131072); // hash: de2f2609b7d4158cb88f84f64901a3ef, width: 128, height: 128
    datas[4][ 4] = texture_data( 512,   32768, 100055168,   32768); // hash: 96947b3f2ea620a01d6fdff092912a55, width: 64, height: 64
    datas[4][ 5] = texture_data( 256,    8192, 100087936,    8192); // hash: 5a3747d336e986a8b13f8130e9810f82, width: 32, height: 32
    datas[4][ 6] = texture_data( 128,    2048, 100096128,    2048); // hash: 0a43e3945958925c7202dc7bed7a28fb, width: 16, height: 16
    datas[4][ 7] = texture_data(  64,     512, 100098176,     512); // hash: f2bec3deca649d33f94e5d50c34c34b0, width: 8, height: 8
    datas[4][ 8] = texture_data(  32,     128, 100098688,     128); // hash: 838b85ee55cd58cd173c84757b13a289, width: 4, height: 4
    datas[4][ 9] = texture_data(  16,      32, 100098816,      32); // hash: 4468b4120baf512adced842a40041405, width: 2, height: 2
    datas[4][10] = texture_data(   8,       8, 100098848,       8); // hash: ea9cf4b87e3051e5a1a80b458f58df15, width: 1, height: 1
    datas[5][ 0] = texture_data(8192, 8388608, 100098856, 8388608); // hash: 7e43c9b640bbf8113099bc1f54a3f4d0, width: 1024, height: 1024
    datas[5][ 1] = texture_data(4096, 2097152, 108487464, 2097152); // hash: 043f9e4a7880bb4bf92ecfd1f8bfe6e1, width: 512, height: 512
    datas[5][ 2] = texture_data(2048,  524288, 110584616,  524288); // hash: fb6cc9718402d5adbfb1b1ed2b777bf9, width: 256, height: 256
    datas[5][ 3] = texture_data(1024,  131072, 111108904,  131072); // hash: ac11b78e531199f5a52bb19c06bf65e3, width: 128, height: 128
    datas[5][ 4] = texture_data( 512,   32768, 111239976,   32768); // hash: 4de8799a3aed4c8d8b9bd9b0f983e1e9, width: 64, height: 64
    datas[5][ 5] = texture_data( 256,    8192, 111272744,    8192); // hash: 592488bca93776e60489232ba93b9415, width: 32, height: 32
    datas[5][ 6] = texture_data( 128,    2048, 111280936,    2048); // hash: 6a47f7e57de4b65a3b6f8441291d4274, width: 16, height: 16
    datas[5][ 7] = texture_data(  64,     512, 111282984,     512); // hash: 4f8116ee9953a5d394c961a00058f844, width: 8, height: 8
    datas[5][ 8] = texture_data(  32,     128, 111283496,     128); // hash: 79e1654473d8ba7d59667cfe71798ab4, width: 4, height: 4
    datas[5][ 9] = texture_data(  16,      32, 111283624,      32); // hash: 2b12f35cb941ce5444d6fe010c71e513, width: 2, height: 2
    datas[5][10] = texture_data(   8,       8, 111283656,       8); // hash: 424dd03dbe2eb284de8a0348871d0704, width: 1, height: 1
    // clang-format on
    return CubeTexture {
        .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::shanghai_bund_lut() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(2048, 1048576, 111283664, 1048576); // hash: 7f7048fcf260a2aa1eae75b62340d74d, width: 512, height: 512
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R16G16_FLOAT,
        .width = 512,
        .height = 512,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::shanghai_bund_irr() const -> CubeTexture {
    decltype(CubeTexture::datas) datas = {};
    // clang-format off
    datas[0][ 0] = texture_data(1024,  131072, 112332240,  131072); // hash: 2e2aa256fff67123a2a4fc3322c42d71, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072, 112463312,  131072); // hash: 3bbc601120883a26ccd37f0a530c0b12, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072, 112594384,  131072); // hash: 45b9b9637ea6e7204764acb1353843db, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072, 112725456,  131072); // hash: d4983dea763c4559d2c168976b5f778d, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072, 112856528,  131072); // hash: 14afe16f16de2e066ebd895b3d834142, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072, 112987600,  131072); // hash: 97d8fac08f450c25bcaa17110f0fa7e9, width: 128, height: 128
    // clang-format on
    return CubeTexture {
        .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
        .width = 128,
        .height = 128,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::shanghai_bund_rad() const -> CubeTexture {
    decltype(CubeTexture::datas) datas = {};
    // clang-format off
    datas[0][ 0] = texture_data(8192, 8388608, 113118672, 8388608); // hash: f1dac2f642724068cfe5a596095cd1af, width: 1024, height: 1024
    datas[0][ 1] = texture_data(4096, 2097152, 121507280, 2097152); // hash: 1c8a26b6a21b45ca2e5a85c6f40427de, width: 512, height: 512
    datas[0][ 2] = texture_data(2048,  524288, 123604432,  524288); // hash: 4d6f669c74778027f6d1906e39178e63, width: 256, height: 256
    datas[0][ 3] = texture_data(1024,  131072, 124128720,  131072); // hash: c4bc3c5d04987935544004eecea8af9a, width: 128, height: 128
    datas[0][ 4] = texture_data( 512,   32768, 124259792,   32768); // hash: 86600ba136ef9907f544b129205ab843, width: 64, height: 64
    datas[0][ 5] = texture_data( 256,    8192, 124292560,    8192); // hash: 54166bee57fd14547e55dcdd21fc8d95, width: 32, height: 32
    datas[0][ 6] = texture_data( 128,    2048, 124300752,    2048); // hash: da8f24bc0157eb77c104242d1709cef9, width: 16, height: 16
    datas[0][ 7] = texture_data(  64,     512, 124302800,     512); // hash: 225f6df30d026df02eba6cc2adbf5275, width: 8, height: 8
    datas[0][ 8] = texture_data(  32,     128, 124303312,     128); // hash: f087a791fb8f4e1347d275d1ee3faf5d, width: 4, height: 4
    datas[0][ 9] = texture_data(  16,      32, 124303440,      32); // hash: 6d1e1eb8148cc2d9a44918a3ca1f827d, width: 2, height: 2
    datas[0][10] = texture_data(   8,       8, 124303472,       8); // hash: 9b6001f2a75c12628d93a3bf0e10d296, width: 1, height: 1
    datas[1][ 0] = texture_data(8192, 8388608, 124303480, 8388608); // hash: 5c262cb25a1e21e5fc9c9e2aacf369b9, width: 1024, height: 1024
    datas[1][ 1] = texture_data(4096, 2097152, 132692088, 2097152); // hash: dc6de75243f0b0a4742616ada7d91ae8, width: 512, height: 512
    datas[1][ 2] = texture_data(2048,  524288, 134789240,  524288); // hash: aeccbe3baca29ea206d9191f20c37e5d, width: 256, height: 256
    datas[1][ 3] = texture_data(1024,  131072, 135313528,  131072); // hash: c41533f8d83dac3f0fbeaff88dc27024, width: 128, height: 128
    datas[1][ 4] = texture_data( 512,   32768, 135444600,   32768); // hash: abe0b4168f2754fe09ad90f9a50f6d37, width: 64, height: 64
    datas[1][ 5] = texture_data( 256,    8192, 135477368,    8192); // hash: 7999d6c0343180d3b23ee37de3cb720b, width: 32, height: 32
    datas[1][ 6] = texture_data( 128,    2048, 135485560,    2048); // hash: 5b6abcb6d2100b59fe8f2f81e203e85f, width: 16, height: 16
    datas[1][ 7] = texture_data(  64,     512, 135487608,     512); // hash: fb70262f76033cc99248d83f46de534e, width: 8, height: 8
    datas[1][ 8] = texture_data(  32,     128, 135488120,     128); // hash: 54d914303dd5eefbefcce3f4c58922ac, width: 4, height: 4
    datas[1][ 9] = texture_data(  16,      32, 135488248,      32); // hash: 26f4fdc1ccac45caa1509b57db7912a3, width: 2, height: 2
    datas[1][10] = texture_data(   8,       8, 135488280,       8); // hash: a70b8d4ec200454162775b5f8b1ec109, width: 1, height: 1
    datas[2][ 0] = texture_data(8192, 8388608, 135488288, 8388608); // hash: 655df6523b827f552d2e77a9b26656cf, width: 1024, height: 1024
    datas[2][ 1] = texture_data(4096, 2097152, 143876896, 2097152); // hash: 36be9339496f43f7eef083712d0a26d4, width: 512, height: 512
    datas[2][ 2] = texture_data(2048,  524288, 145974048,  524288); // hash: 80897a861e3de3a02552a919606c69ef, width: 256, height: 256
    datas[2][ 3] = texture_data(1024,  131072, 146498336,  131072); // hash: 6086480b26d0b7ce046f1ca6d5ef8693, width: 128, height: 128
    datas[2][ 4] = texture_data( 512,   32768, 146629408,   32768); // hash: 11a09eaf78b226c2fe91a2b32a47d9bc, width: 64, height: 64
    datas[2][ 5] = texture_data( 256,    8192, 146662176,    8192); // hash: 71213a81faa787f7fb9b4d5ed51732b7, width: 32, height: 32
    datas[2][ 6] = texture_data( 128,    2048, 146670368,    2048); // hash: 4af1ccf6f5c0a8fabff467c323581bcd, width: 16, height: 16
    datas[2][ 7] = texture_data(  64,     512, 146672416,     512); // hash: 44594d2e62e9535b5be0f465f8536a46, width: 8, height: 8
    datas[2][ 8] = texture_data(  32,     128, 146672928,     128); // hash: b42139ed8d524ec18340525524b69e77, width: 4, height: 4
    datas[2][ 9] = texture_data(  16,      32, 146673056,      32); // hash: 8284584e26ed0c92b5d71c5d6ff3bf9d, width: 2, height: 2
    datas[2][10] = texture_data(   8,       8, 146673088,       8); // hash: aec97c034320ee9b0b2a0dd4d5171e60, width: 1, height: 1
    datas[3][ 0] = texture_data(8192, 8388608, 146673096, 8388608); // hash: 95b2e9256e59cae29c48f2d7d4ede70b, width: 1024, height: 1024
    datas[3][ 1] = texture_data(4096, 2097152, 155061704, 2097152); // hash: bdcbdc9f5021c87bc74cc2e79620c711, width: 512, height: 512
    datas[3][ 2] = texture_data(2048,  524288, 157158856,  524288); // hash: 8f597032a29f04cc7c266b1473189190, width: 256, height: 256
    datas[3][ 3] = texture_data(1024,  131072, 157683144,  131072); // hash: a522798ddbace386b9748c650d0bd589, width: 128, height: 128
    datas[3][ 4] = texture_data( 512,   32768, 157814216,   32768); // hash: b39d74423230d4ef34f4a01354f06dbb, width: 64, height: 64
    datas[3][ 5] = texture_data( 256,    8192, 157846984,    8192); // hash: 16b172ecdc77d53f3f4f910499416e22, width: 32, height: 32
    datas[3][ 6] = texture_data( 128,    2048, 157855176,    2048); // hash: 8cd998d0b667d36d9d17a7db82b30a88, width: 16, height: 16
    datas[3][ 7] = texture_data(  64,     512, 157857224,     512); // hash: f39fa59576ca888db963623568bf0ea5, width: 8, height: 8
    datas[3][ 8] = texture_data(  32,     128, 157857736,     128); // hash: 2add947eb14fada697697931f3a250c1, width: 4, height: 4
    datas[3][ 9] = texture_data(  16,      32, 157857864,      32); // hash: b2a1d97d48c8032f37cf36ae9cb35ea4, width: 2, height: 2
    datas[3][10] = texture_data(   8,       8, 157857896,       8); // hash: 563b13464e49eab1a96147576b00c6bc, width: 1, height: 1
    datas[4][ 0] = texture_data(8192, 8388608, 157857904, 8388608); // hash: 17a3f09f6461513ab3978b63fab6b79d, width: 1024, height: 1024
    datas[4][ 1] = texture_data(4096, 2097152, 166246512, 2097152); // hash: 5dfa489e2f014ebff7c26136bf7930e3, width: 512, height: 512
    datas[4][ 2] = texture_data(2048,  524288, 168343664,  524288); // hash: d30d23f72d1d2872a6fbb177c397315f, width: 256, height: 256
    datas[4][ 3] = texture_data(1024,  131072, 168867952,  131072); // hash: 7ed58d6f690b7d7d1145fb9a1844a2f1, width: 128, height: 128
    datas[4][ 4] = texture_data( 512,   32768, 168999024,   32768); // hash: 4fd24eb9b5086d7b02bf0cb6680cb56a, width: 64, height: 64
    datas[4][ 5] = texture_data( 256,    8192, 169031792,    8192); // hash: 20cd0518fb2e562f63cc76472572af74, width: 32, height: 32
    datas[4][ 6] = texture_data( 128,    2048, 169039984,    2048); // hash: fe758f7b4f6cdd6ce36f3b63bbccebb4, width: 16, height: 16
    datas[4][ 7] = texture_data(  64,     512, 169042032,     512); // hash: 573f4f668a5c39117daee74b730bd470, width: 8, height: 8
    datas[4][ 8] = texture_data(  32,     128, 169042544,     128); // hash: 1704840cfb6d56378118f39d7378bf49, width: 4, height: 4
    datas[4][ 9] = texture_data(  16,      32, 169042672,      32); // hash: a226a62cb18192d068f68dea3b3c2d05, width: 2, height: 2
    datas[4][10] = texture_data(   8,       8, 169042704,       8); // hash: 16be76e995343fb04ff0c00918a40e90, width: 1, height: 1
    datas[5][ 0] = texture_data(8192, 8388608, 169042712, 8388608); // hash: 4b3393b50e560bef692124e8d132134d, width: 1024, height: 1024
    datas[5][ 1] = texture_data(4096, 2097152, 177431320, 2097152); // hash: 1eef69956e198f5357901dd07f464a6d, width: 512, height: 512
    datas[5][ 2] = texture_data(2048,  524288, 179528472,  524288); // hash: 1af3c6af6280f1792a657deb5a923955, width: 256, height: 256
    datas[5][ 3] = texture_data(1024,  131072, 180052760,  131072); // hash: 23fc8e43c4bbe4ddef07fc5fd6bd5e85, width: 128, height: 128
    datas[5][ 4] = texture_data( 512,   32768, 180183832,   32768); // hash: dc8659af1986cd692f12566594ae6da1, width: 64, height: 64
    datas[5][ 5] = texture_data( 256,    8192, 180216600,    8192); // hash: a101bf90ff26cdbab7d36ae740f5416e, width: 32, height: 32
    datas[5][ 6] = texture_data( 128,    2048, 180224792,    2048); // hash: e43e67959b0bbe90014604b650cbac75, width: 16, height: 16
    datas[5][ 7] = texture_data(  64,     512, 180226840,     512); // hash: 18701ec219c94b89f728866e3b401cb8, width: 8, height: 8
    datas[5][ 8] = texture_data(  32,     128, 180227352,     128); // hash: 1a2d5e4bab40436129b0906c2540abb1, width: 4, height: 4
    datas[5][ 9] = texture_data(  16,      32, 180227480,      32); // hash: 0064208c729ac8dfa2c4c6a8dadbeea9, width: 2, height: 2
    datas[5][10] = texture_data(   8,       8, 180227512,       8); // hash: 477b556f26425c0564e42f5471a29448, width: 1, height: 1
    // clang-format on
    return CubeTexture {
        .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::industrial_sunset_02_puresky_irr() const -> CubeTexture {
    decltype(CubeTexture::datas) datas = {};
    // clang-format off
    datas[0][ 0] = texture_data(1024,  131072, 180227520,  131072); // hash: 14bace1323fdb1b8093dfd31df5652a4, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072, 180358592,  131072); // hash: be316532626188efaffe393800a2b463, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072, 180489664,  131072); // hash: 5f9bc366b6a4a7fb368796936dc50d94, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072, 180620736,  131072); // hash: 6f95ec7a8f2c6167a11879a2af8972d5, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072, 180751808,  131072); // hash: 0e69d9da1b68db7535563c39e2574521, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072, 180882880,  131072); // hash: c8dd89cfcc42f96602486d5c12a952c4, width: 128, height: 128
    // clang-format on
    return CubeTexture {
        .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
        .width = 128,
        .height = 128,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::roboto_medium_font() const -> Font {
    return Font {
        .ascender = 0.9277344f,
        .descender = -0.24414062f,
        .space_advance = 0.24902344f,
        // hash: 256bbea6356b2b592731c2cb48958d7f
        .glyphs = transmuted_span<Glyph>(181013952, 94),
    };
}

auto Assets::roboto_medium_mesh() const -> Mesh {
    // vertex_count: 35628
    // face_count: 23448
    // submesh_count: 94
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
        // hash: 29374dddcd21f32bd4d1c96ff182a0ae
        .vertices = transmuted_span<Vertex>(181016960, 35628),
        // hash: 022ec4951ed71315112a36ddd2c7ecf2
        .indices = transmuted_span<Index>(182727104, 70344),
        // hash: 8384a220e7f3e90e627e9774b7dbd4c1
        .submeshes = transmuted_span<Submesh>(183008480, 94),
    };
}

auto Assets::lightsaber_mesh() const -> Mesh {
    // vertex_count: 371
    // face_count: 224
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
        // hash: 2ed114c3892782da8bc5e2c500c527cb
        .vertices = transmuted_span<Vertex>(183009608, 371),
        // hash: 63165375d063998e676f3993a3de1c90
        .indices = transmuted_span<Index>(183027416, 672),
        // hash: 4de1f7183f3facd76905bc4fab9e4e79
        .submeshes = transmuted_span<Submesh>(183030104, 1),
    };
}

auto Assets::lightsaber_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(   4,       4, 183030116,       4); // hash: a03472865ceee4a8780952daaa607a20, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1,
        .height = 1,
        .mip_count = 1,
        .datas = datas,
    };
}

auto Assets::lightsaber_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.5f,
        .alpha_mode = AlphaMode::Opaque,
    };
}

auto Assets::grass_mesh() const -> Mesh {
    // vertex_count: 7468
    // face_count: 5440
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
        // hash: ce365e55889bb1a329b4528219fbcb39
        .vertices = transmuted_span<Vertex>(183030120, 7468),
        // hash: 5e0e33f101aa3ae1b7be6d552a126fe9
        .indices = transmuted_span<Index>(183388584, 16320),
        // hash: c6415d8cb3741bdc20a1c9e7152aaa87
        .submeshes = transmuted_span<Submesh>(183453864, 1),
    };
}

auto Assets::grass_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304, 183453876, 4194304); // hash: 8ba5e6c4c962142b6323c9b718806667, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576, 187648180, 1048576); // hash: 535c31d03cb632317d06433beff89448, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144, 188696756,  262144); // hash: 23a677d2c655fa098c981da6fc12e7a6, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536, 188958900,   65536); // hash: 0afea75560cd4eace0ffe2007bdda0af, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384, 189024436,   16384); // hash: 0d23e0d7cd4be6aed9c0e4cb80d54bd4, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096, 189040820,    4096); // hash: 0b4e5fd48795c7b07206e38c42debfb6, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024, 189044916,    1024); // hash: 05f6d3e4041bf8819cc0f7a1e7a28094, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256, 189045940,     256); // hash: b52d391bbabaf303be3a18a0f44e3ba6, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64, 189046196,      64); // hash: a44d4ba5c1df725dabef70679c78a94c, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16, 189046260,      16); // hash: e3cd0daafab292bf484f67daa63cfbef, width: 2, height: 2
    datas[10] = texture_data(   4,       4, 189046276,       4); // hash: 2426906e3a0c17efd9b0748b57fe5f46, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::grass_material() const -> Material {
    return Material {
        .alpha_cutoff = 0.6913036f,
        .alpha_mode = AlphaMode::Mask,
    };
}

auto Shaders::load() -> void {
    // hash: f0b35766d4c9b290a2a9f888c337b134
    ZoneScoped;
    _data = read_whole_file("fb_buffet_shaders.bin");
    FB_ASSERT(_data.size() == 236220);
}

// shader_hash: d5c7057e7dc4e62ece119084aa759e6f
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: d5c7057e7dc4e62ece119084aa759e6f.pdb
; shader hash: d5c7057e7dc4e62ece119084aa759e6f
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @background_vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @background_vs, !"background_vs", !5, null, !16}
!5 = !{!6, !11, null}
!6 = !{!7, !10}
!7 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !8, i8 0, i32 1, i8 1, i32 0, i8 0, !9}
!8 = !{i32 0}
!9 = !{i32 3, i32 1}
!10 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !8, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 0, i64 8388864}
*/
auto Shaders::cards_background_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 3296);
}

// shader_hash: 5873bd0f4bfff2b3acae35e080aa87ff
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 33
// float_instruction_count: 6
// texture_bias_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 5873bd0f4bfff2b3acae35e080aa87ff.pdb
; shader hash: 5873bd0f4bfff2b3acae35e080aa87ff
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_background_bindings
; {
;
;   struct g_background_bindings
;   {
;
;       struct struct.BackgroundBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint texture;                             ; Offset:    4
;       
;       } g_background_bindings;                      ; Offset:    0
;
;   
;   } g_background_bindings;                          ; Offset:    0 Size:     8
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_background_bindings             cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;   output 3 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_background_bindings = type { %struct.BackgroundBindings }
%struct.BackgroundBindings = type { i32, i32 }

define void @background_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 8 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %14 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %15 = extractvalue %dx.types.CBufRet.f32 %14, 0
  %16 = fsub fast float 1.000000e+00, %15
  %17 = fsub fast float %16, %15
  %18 = fmul fast float %17, %3
  %19 = fmul fast float %17, %4
  %20 = fadd fast float %18, %15
  %21 = fadd fast float %19, %15
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %24 = call %dx.types.ResRet.f32 @dx.op.sampleBias.f32(i32 61, %dx.types.Handle %11, %dx.types.Handle %13, float %20, float %21, float undef, float undef, i32 0, i32 0, i32 undef, float %23, float undef)  ; SampleBias(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,bias,clamp)
  %25 = extractvalue %dx.types.ResRet.f32 %24, 0
  %26 = extractvalue %dx.types.ResRet.f32 %24, 1
  %27 = extractvalue %dx.types.ResRet.f32 %24, 2
  %28 = extractvalue %dx.types.ResRet.f32 %24, 3
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %25)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %26)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %27)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %28)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleBias.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_background_bindings* undef, !"", i32 0, i32 0, i32 1, i32 8, null}
!6 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!7 = !{void ()* @background_ps, !"background_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!13 = !{i32 3, i32 3}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 3229614336}
*/
auto Shaders::cards_background_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(3296, 4480);
}

// shader_hash: 0b7ccc194e0c25f939d634d0dcf0a9c5
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 64
// float_instruction_count: 12
// texture_load_instructions: 4
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: 0b7ccc194e0c25f939d634d0dcf0a9c5.pdb
; shader hash: 0b7ccc194e0c25f939d634d0dcf0a9c5
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
; cbuffer g_draw_bindings
; {
;
;   struct g_draw_bindings
;   {
;
;       struct struct.DrawBindings
;       {
;
;           uint card_index;                          ; Offset:    0
;           uint constants;                           ; Offset:    4
;           uint cards;                               ; Offset:    8
;           uint vertices;                            ; Offset:   12
;           uint texture;                             ; Offset:   16
;       
;       } g_draw_bindings;                            ; Offset:    0
;
;   
;   } g_draw_bindings;                                ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_draw_bindings                   cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_draw_bindings = type { %struct.DrawBindings }
%struct.DrawBindings = type { i32, i32, i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 2
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 16 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=16>
  %11 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %12 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %11, i32 0, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %13 = extractvalue %dx.types.ResRet.f32 %12, 0
  %14 = extractvalue %dx.types.ResRet.f32 %12, 1
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %11, i32 8, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.CBufRet.i32 %4, 3
  %19 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %18, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %20 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %19, %dx.types.ResourceProperties { i32 524, i32 16 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=16>
  %21 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %20, i32 %3, i32 0, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %22 = extractvalue %dx.types.ResRet.f32 %21, 0
  %23 = extractvalue %dx.types.ResRet.f32 %21, 1
  %24 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %20, i32 %3, i32 8, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %25 = extractvalue %dx.types.ResRet.f32 %24, 0
  %26 = extractvalue %dx.types.ResRet.f32 %24, 1
  %27 = fmul fast float %22, %16
  %28 = fmul fast float %23, %17
  %29 = fadd fast float %27, %13
  %30 = fadd fast float %28, %14
  %31 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %32 = extractvalue %dx.types.CBufRet.f32 %31, 0
  %33 = extractvalue %dx.types.CBufRet.f32 %31, 1
  %34 = extractvalue %dx.types.CBufRet.f32 %31, 2
  %35 = extractvalue %dx.types.CBufRet.f32 %31, 3
  %36 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %37 = extractvalue %dx.types.CBufRet.f32 %36, 0
  %38 = extractvalue %dx.types.CBufRet.f32 %36, 1
  %39 = extractvalue %dx.types.CBufRet.f32 %36, 2
  %40 = extractvalue %dx.types.CBufRet.f32 %36, 3
  %41 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %42 = extractvalue %dx.types.CBufRet.f32 %41, 0
  %43 = extractvalue %dx.types.CBufRet.f32 %41, 1
  %44 = extractvalue %dx.types.CBufRet.f32 %41, 2
  %45 = extractvalue %dx.types.CBufRet.f32 %41, 3
  %46 = fmul fast float %32, %29
  %47 = call float @dx.op.tertiary.f32(i32 46, float %37, float %30, float %46)  ; FMad(a,b,c)
  %48 = fadd fast float %42, %47
  %49 = fmul fast float %33, %29
  %50 = call float @dx.op.tertiary.f32(i32 46, float %38, float %30, float %49)  ; FMad(a,b,c)
  %51 = fadd fast float %50, %43
  %52 = fmul fast float %34, %29
  %53 = call float @dx.op.tertiary.f32(i32 46, float %39, float %30, float %52)  ; FMad(a,b,c)
  %54 = fadd fast float %53, %44
  %55 = fmul fast float %35, %29
  %56 = call float @dx.op.tertiary.f32(i32 46, float %40, float %30, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %45
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %48)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %51)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %54)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %25)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %26)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_draw_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{[7 x i32] [i32 5, i32 6, i32 63, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !19}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !18}
!18 = !{i32 3, i32 3}
!19 = !{i32 0, i64 1082130688}
*/
auto Shaders::cards_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(7776, 4896);
}

// shader_hash: 3b57576fb46ab00f0187eab225ba0cce
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 3b57576fb46ab00f0187eab225ba0cce.pdb
; shader hash: 3b57576fb46ab00f0187eab225ba0cce
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_draw_bindings
; {
;
;   struct g_draw_bindings
;   {
;
;       struct struct.DrawBindings
;       {
;
;           uint card_index;                          ; Offset:    0
;           uint constants;                           ; Offset:    4
;           uint cards;                               ; Offset:    8
;           uint vertices;                            ; Offset:   12
;           uint texture;                             ; Offset:   16
;       
;       } g_draw_bindings;                            ; Offset:    0
;
;   
;   } g_draw_bindings;                                ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_draw_bindings                   cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;   output 3 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_draw_bindings = type { %struct.DrawBindings }
%struct.DrawBindings = type { i32, i32, i32, i32, i32 }

define void @draw_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %11 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %8, %dx.types.Handle %10, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = extractvalue %dx.types.ResRet.f32 %11, 3
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_draw_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!7 = !{void ()* @draw_ps, !"draw_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!13 = !{i32 3, i32 3}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 3229614336}
*/
auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(12672, 4256);
}

// shader_hash: 15cbce87e26cec13f6b8977c797f0cd9
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: 15cbce87e26cec13f6b8977c797f0cd9.pdb
; shader hash: 15cbce87e26cec13f6b8977c797f0cd9
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @clear_vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @clear_vs, !"clear_vs", !5, null, !16}
!5 = !{!6, !11, null}
!6 = !{!7, !10}
!7 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !8, i8 0, i32 1, i8 1, i32 0, i8 0, !9}
!8 = !{i32 0}
!9 = !{i32 3, i32 1}
!10 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !8, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 0, i64 8388864}
*/
auto Shaders::conras_clear_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(16928, 3268);
}

// shader_hash: c358132fa94fcef8c301e912a582faa6
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// instruction_count: 21
// float_instruction_count: 2
// int_instruction_count: 2
// texture_store_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xy  
; ATTRIBUTE                0   xy          1     NONE   float       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: c358132fa94fcef8c301e912a582faa6.pdb
; shader hash: c358132fa94fcef8c301e912a582faa6
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint raster_buffer;                       ; Offset:    4
;           uint conservative_rasterization;          ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 0
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @clear_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 4108, i32 12 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=12>
  %12 = fptoui float %3 to i32
  %13 = fptoui float %4 to i32
  %14 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %15 = extractvalue %dx.types.CBufRet.i32 %14, 0
  %16 = mul i32 %15, %13
  %17 = add i32 %16, %12
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %11, i32 %17, i32 0, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %11, i32 %17, i32 4, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %11, i32 %17, i32 8, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[2 x i32] [i32 6, i32 0]}
!7 = !{void ()* @clear_ps, !"clear_ps", !8, !3, !14}
!8 = !{!9, null, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 3}
!13 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, null}
!14 = !{i32 0, i64 9672065280}
*/
auto Shaders::conras_clear_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(20196, 3892);
}

// shader_hash: 4d8e25edf2897dcad66e13139f5f9008
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 60
// float_instruction_count: 8
// int_instruction_count: 6
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
;
; shader debug name: 4d8e25edf2897dcad66e13139f5f9008.pdb
; shader hash: 4d8e25edf2897dcad66e13139f5f9008
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint raster_buffer;                       ; Offset:    4
;           uint conservative_rasterization;          ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

@STAR_VERTICES.v.1dim = internal constant [33 x float] [float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00, float 5.000000e-01, float 0.000000e+00, float 0xBFBE353F80000000, float 0x3FC4BC6A80000000, float 0.000000e+00, float 0xBFDE76C8C0000000, float 0x3FC3D70A40000000, float 0.000000e+00, float 0xBFC851EB80000000, float 0xBFAFA43FE0000000, float 0.000000e+00, float 0xBFD2D0E560000000, float 0xBFD9EB8520000000, float 0.000000e+00, float 0.000000e+00, float 0xBFC99999A0000000, float 0.000000e+00, float 0x3FD2D0E560000000, float 0xBFD9EB8520000000, float 0.000000e+00, float 0x3FC851EB80000000, float 0xBFAFA43FE0000000, float 0.000000e+00, float 0x3FDE76C8C0000000, float 0x3FC3D70A40000000, float 0.000000e+00, float 0x3FBE353F80000000, float 0x3FC4BC6A80000000, float 0.000000e+00], align 4

define void @raster_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = mul i32 %3, 3
  %9 = add i32 0, %8
  %10 = getelementptr [33 x float], [33 x float]* @STAR_VERTICES.v.1dim, i32 0, i32 %9
  %11 = load float, float* %10, align 4
  %12 = mul i32 %3, 3
  %13 = add i32 1, %12
  %14 = getelementptr [33 x float], [33 x float]* @STAR_VERTICES.v.1dim, i32 0, i32 %13
  %15 = load float, float* %14, align 4
  %16 = mul i32 %3, 3
  %17 = add i32 2, %16
  %18 = getelementptr [33 x float], [33 x float]* @STAR_VERTICES.v.1dim, i32 0, i32 %17
  %19 = load float, float* %18, align 4
  %20 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %21 = extractvalue %dx.types.CBufRet.f32 %20, 0
  %22 = extractvalue %dx.types.CBufRet.f32 %20, 1
  %23 = extractvalue %dx.types.CBufRet.f32 %20, 2
  %24 = extractvalue %dx.types.CBufRet.f32 %20, 3
  %25 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %26 = extractvalue %dx.types.CBufRet.f32 %25, 0
  %27 = extractvalue %dx.types.CBufRet.f32 %25, 1
  %28 = extractvalue %dx.types.CBufRet.f32 %25, 2
  %29 = extractvalue %dx.types.CBufRet.f32 %25, 3
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 2
  %34 = extractvalue %dx.types.CBufRet.f32 %30, 3
  %35 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %36 = extractvalue %dx.types.CBufRet.f32 %35, 0
  %37 = extractvalue %dx.types.CBufRet.f32 %35, 1
  %38 = extractvalue %dx.types.CBufRet.f32 %35, 2
  %39 = extractvalue %dx.types.CBufRet.f32 %35, 3
  %40 = fmul fast float %21, %11
  %41 = call float @dx.op.tertiary.f32(i32 46, float %26, float %15, float %40)  ; FMad(a,b,c)
  %42 = call float @dx.op.tertiary.f32(i32 46, float %31, float %19, float %41)  ; FMad(a,b,c)
  %43 = fadd fast float %42, %36
  %44 = fmul fast float %22, %11
  %45 = call float @dx.op.tertiary.f32(i32 46, float %27, float %15, float %44)  ; FMad(a,b,c)
  %46 = call float @dx.op.tertiary.f32(i32 46, float %32, float %19, float %45)  ; FMad(a,b,c)
  %47 = fadd fast float %46, %37
  %48 = fmul fast float %23, %11
  %49 = call float @dx.op.tertiary.f32(i32 46, float %28, float %15, float %48)  ; FMad(a,b,c)
  %50 = call float @dx.op.tertiary.f32(i32 46, float %33, float %19, float %49)  ; FMad(a,b,c)
  %51 = fadd fast float %50, %38
  %52 = fmul fast float %24, %11
  %53 = call float @dx.op.tertiary.f32(i32 46, float %29, float %15, float %52)  ; FMad(a,b,c)
  %54 = call float @dx.op.tertiary.f32(i32 46, float %34, float %19, float %53)  ; FMad(a,b,c)
  %55 = fadd fast float %54, %39
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %43)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %47)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %51)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %55)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[7 x i32] [i32 5, i32 4, i32 15, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @raster_vs, !"raster_vs", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 1082130688}
*/
auto Shaders::conras_raster_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(24088, 4800);
}

// shader_hash: e2c2a101c144512def6d531bc2823c54
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 1
// instruction_count: 19
// float_instruction_count: 2
// int_instruction_count: 2
// interlocked_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: e2c2a101c144512def6d531bc2823c54.pdb
; shader hash: e2c2a101c144512def6d531bc2823c54
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint raster_buffer;                       ; Offset:    4
;           uint conservative_rasterization;          ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 4, outputs: 0
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @raster_cr_off_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 4108, i32 12 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=12>
  %12 = fptoui float %3 to i32
  %13 = fptoui float %4 to i32
  %14 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %15 = extractvalue %dx.types.CBufRet.i32 %14, 0
  %16 = mul i32 %15, %13
  %17 = add i32 %16, %12
  %18 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %11, i32 0, i32 %17, i32 0, i32 undef, i32 1)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind
declare i32 @dx.op.atomicBinOp.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[2 x i32] [i32 4, i32 0]}
!7 = !{void ()* @raster_cr_off_ps, !"raster_cr_off_ps", !8, !3, !13}
!8 = !{!9, null, null}
!9 = !{!10}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 3}
!13 = !{i32 0, i64 9672065280}
*/
auto Shaders::conras_raster_cr_off_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(28888, 3752);
}

// shader_hash: 0024cdc5f5e655e5082dfc0c32cbf5c2
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 1
// instruction_count: 22
// float_instruction_count: 2
// int_instruction_count: 2
// uint_instruction_count: 1
// interlocked_instructions: 2
/* disassembly:
;
; Note: shader requires additional functionality:
;       PS Inner Coverage
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: 0024cdc5f5e655e5082dfc0c32cbf5c2.pdb
; shader hash: 0024cdc5f5e655e5082dfc0c32cbf5c2
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint raster_buffer;                       ; Offset:    4
;           uint conservative_rasterization;          ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 4, outputs: 0
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @raster_cr_on_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call i32 @dx.op.innerCoverage.i32(i32 92)  ; InnerCoverage()
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 4108, i32 12 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=12>
  %13 = fptoui float %3 to i32
  %14 = fptoui float %4 to i32
  %15 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %16 = extractvalue %dx.types.CBufRet.i32 %15, 0
  %17 = mul i32 %16, %14
  %18 = add i32 %17, %13
  %19 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %12, i32 0, i32 %18, i32 4, i32 undef, i32 1)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  %20 = and i32 %5, 1
  %21 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %12, i32 0, i32 %18, i32 8, i32 undef, i32 %20)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.innerCoverage.i32(i32) #0

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind
declare i32 @dx.op.atomicBinOp.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[2 x i32] [i32 4, i32 0]}
!7 = !{void ()* @raster_cr_on_ps, !"raster_cr_on_ps", !8, !3, !13}
!8 = !{!9, null, null}
!9 = !{!10}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 3}
!13 = !{i32 0, i64 9672066304}
*/
auto Shaders::conras_raster_cr_on_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(32640, 3884);
}

// shader_hash: af4151a88d2121bea71ce4681c211f03
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: af4151a88d2121bea71ce4681c211f03.pdb
; shader hash: af4151a88d2121bea71ce4681c211f03
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @display_vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @display_vs, !"display_vs", !5, null, !16}
!5 = !{!6, !11, null}
!6 = !{!7, !10}
!7 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !8, i8 0, i32 1, i8 1, i32 0, i8 0, !9}
!8 = !{i32 0}
!9 = !{i32 3, i32 1}
!10 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !8, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 0, i64 8388864}
*/
auto Shaders::conras_display_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(36524, 3284);
}

// shader_hash: 795526289feb5fcb97abc2aacf5ee045
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 48
// float_instruction_count: 12
// int_instruction_count: 5
// uint_instruction_count: 1
// texture_load_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 795526289feb5fcb97abc2aacf5ee045.pdb
; shader hash: 795526289feb5fcb97abc2aacf5ee045
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint raster_buffer;                       ; Offset:    4
;           uint conservative_rasterization;          ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @display_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 804 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 12, i32 12 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=12>
  %12 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %13 = extractvalue %dx.types.CBufRet.i32 %12, 0
  %14 = extractvalue %dx.types.CBufRet.i32 %12, 1
  %15 = uitofp i32 %13 to float
  %16 = uitofp i32 %14 to float
  %17 = fmul fast float %15, %3
  %18 = fmul fast float %16, %4
  %19 = fptoui float %17 to i32
  %20 = fptoui float %18 to i32
  %21 = mul i32 %20, %13
  %22 = add i32 %21, %19
  %23 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %11, i32 %22, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %24 = extractvalue %dx.types.ResRet.i32 %23, 1
  %25 = extractvalue %dx.types.ResRet.i32 %23, 2
  %26 = extractvalue %dx.types.CBufRet.i32 %5, 2
  switch i32 %26, label %40 [
    i32 0, label %27
    i32 1, label %30
    i32 2, label %33
    i32 3, label %35
  ]

; <label>:27                                      ; preds = %0
  %28 = extractvalue %dx.types.ResRet.i32 %23, 0
  %29 = uitofp i32 %28 to float
  br label %40

; <label>:30                                      ; preds = %0
  %31 = call i32 @dx.op.binary.i32(i32 40, i32 %24, i32 1)  ; UMin(a,b)
  %32 = uitofp i32 %31 to float
  br label %40

; <label>:33                                      ; preds = %0
  %34 = uitofp i32 %25 to float
  br label %40

; <label>:35                                      ; preds = %0
  %36 = sub nsw i32 %24, %25
  %37 = call i32 @dx.op.binary.i32(i32 37, i32 %36, i32 0)  ; IMax(a,b)
  %38 = call i32 @dx.op.binary.i32(i32 38, i32 %37, i32 1)  ; IMin(a,b)
  %39 = sitofp i32 %38 to float
  br label %40

; <label>:40                                      ; preds = %35, %33, %30, %27, %0
  %41 = phi float [ undef, %0 ], [ %39, %35 ], [ %34, %33 ], [ %32, %30 ], [ %29, %27 ]
  %42 = fmul fast float %41, %3
  %43 = fmul fast float %41, %4
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %42)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %43)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare i32 @dx.op.binary.i32(i32, i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 6, i32 6]}
!7 = !{void ()* @display_ps, !"display_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!13 = !{i32 3, i32 3}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 1082130688}
*/
auto Shaders::conras_display_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(39808, 4516);
}

// shader_hash: ff34f81a9b2fbd77a202421ef918b893
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 94
// float_instruction_count: 20
// texture_load_instructions: 4
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyzw        3     NONE   float   xyzw
; ATTRIBUTE                3   xyz         4     NONE   float   xyz 
; ATTRIBUTE                4   xyz         5     NONE   float   xyz 
;
; shader debug name: ff34f81a9b2fbd77a202421ef918b893.pdb
; shader hash: ff34f81a9b2fbd77a202421ef918b893
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
; ATTRIBUTE                3                 linear       
; ATTRIBUTE                4                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint base_color_texture;                  ; Offset:    8
;           uint normal_texture;                      ; Offset:   12
;           uint metallic_roughness_texture;          ; Offset:   16
;           uint sampler;                             ; Offset:   20
;           uint lut_texture;                         ; Offset:   24
;           uint irr_texture;                         ; Offset:   28
;           uint rad_texture;                         ; Offset:   32
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    36
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 23
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;   output 15 depends on inputs: { 0 }
;   output 16 depends on inputs: { 0 }
;   output 17 depends on inputs: { 0 }
;   output 18 depends on inputs: { 0 }
;   output 20 depends on inputs: { 0 }
;   output 21 depends on inputs: { 0 }
;   output 22 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 36 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 708 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %23 = extractvalue %dx.types.ResRet.f32 %22, 0
  %24 = extractvalue %dx.types.ResRet.f32 %22, 1
  %25 = extractvalue %dx.types.ResRet.f32 %22, 2
  %26 = extractvalue %dx.types.ResRet.f32 %22, 3
  %27 = fmul fast float %25, %17
  %28 = fmul fast float %24, %18
  %29 = fsub fast float %27, %28
  %30 = fmul fast float %23, %18
  %31 = fmul fast float %25, %16
  %32 = fsub fast float %30, %31
  %33 = fmul fast float %24, %16
  %34 = fmul fast float %23, %17
  %35 = fsub fast float %33, %34
  %36 = fmul fast float %29, %26
  %37 = fmul fast float %32, %26
  %38 = fmul fast float %35, %26
  %39 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %40 = extractvalue %dx.types.CBufRet.f32 %39, 0
  %41 = extractvalue %dx.types.CBufRet.f32 %39, 1
  %42 = extractvalue %dx.types.CBufRet.f32 %39, 2
  %43 = extractvalue %dx.types.CBufRet.f32 %39, 3
  %44 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %45 = extractvalue %dx.types.CBufRet.f32 %44, 0
  %46 = extractvalue %dx.types.CBufRet.f32 %44, 1
  %47 = extractvalue %dx.types.CBufRet.f32 %44, 2
  %48 = extractvalue %dx.types.CBufRet.f32 %44, 3
  %49 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %50 = extractvalue %dx.types.CBufRet.f32 %49, 0
  %51 = extractvalue %dx.types.CBufRet.f32 %49, 1
  %52 = extractvalue %dx.types.CBufRet.f32 %49, 2
  %53 = extractvalue %dx.types.CBufRet.f32 %49, 3
  %54 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %55 = extractvalue %dx.types.CBufRet.f32 %54, 0
  %56 = extractvalue %dx.types.CBufRet.f32 %54, 1
  %57 = extractvalue %dx.types.CBufRet.f32 %54, 2
  %58 = extractvalue %dx.types.CBufRet.f32 %54, 3
  %59 = fmul fast float %40, %12
  %60 = call float @dx.op.tertiary.f32(i32 46, float %45, float %13, float %59)  ; FMad(a,b,c)
  %61 = call float @dx.op.tertiary.f32(i32 46, float %50, float %14, float %60)  ; FMad(a,b,c)
  %62 = fadd fast float %61, %55
  %63 = fmul fast float %41, %12
  %64 = call float @dx.op.tertiary.f32(i32 46, float %46, float %13, float %63)  ; FMad(a,b,c)
  %65 = call float @dx.op.tertiary.f32(i32 46, float %51, float %14, float %64)  ; FMad(a,b,c)
  %66 = fadd fast float %65, %56
  %67 = fmul fast float %42, %12
  %68 = call float @dx.op.tertiary.f32(i32 46, float %47, float %13, float %67)  ; FMad(a,b,c)
  %69 = call float @dx.op.tertiary.f32(i32 46, float %52, float %14, float %68)  ; FMad(a,b,c)
  %70 = fadd fast float %69, %57
  %71 = fmul fast float %43, %12
  %72 = call float @dx.op.tertiary.f32(i32 46, float %48, float %13, float %71)  ; FMad(a,b,c)
  %73 = call float @dx.op.tertiary.f32(i32 46, float %53, float %14, float %72)  ; FMad(a,b,c)
  %74 = fadd fast float %73, %58
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %62)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %66)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %70)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %74)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %23)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %24)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %25)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 3, float %26)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 0, float %36)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 1, float %37)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 2, float %38)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 5, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 5, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 5, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 36, null}
!6 = !{[7 x i32] [i32 5, i32 23, i32 7861119, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !28}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22, !24, !26}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 4, i32 3, i8 0, !16}
!23 = !{i32 2}
!24 = !{i32 4, !"ATTRIBUTE", i8 9, i8 0, !25, i8 2, i32 1, i8 3, i32 4, i8 0, !18}
!25 = !{i32 3}
!26 = !{i32 5, !"ATTRIBUTE", i8 9, i8 0, !27, i8 2, i32 1, i8 3, i32 5, i8 0, !18}
!27 = !{i32 4}
!28 = !{i32 0, i64 1082130688}
*/
auto Shaders::crate_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(44324, 5788);
}

// shader_hash: 3ffa6f49f9d1275666a705cb1fcf8756
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 6
// output_parameters: 1
// instruction_count: 344
// dynamic_flow_control_count: 1
// float_instruction_count: 219
// int_instruction_count: 1
// uint_instruction_count: 1
// texture_normal_instructions: 6
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyzw        3     NONE   float   xyz 
; ATTRIBUTE                3   xyz         4     NONE   float   xyz 
; ATTRIBUTE                4   xyz         5     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 3ffa6f49f9d1275666a705cb1fcf8756.pdb
; shader hash: 3ffa6f49f9d1275666a705cb1fcf8756
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
; ATTRIBUTE                3                 linear       
; ATTRIBUTE                4                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint base_color_texture;                  ; Offset:    8
;           uint normal_texture;                      ; Offset:   12
;           uint metallic_roughness_texture;          ; Offset:   16
;           uint sampler;                             ; Offset:   20
;           uint lut_texture;                         ; Offset:   24
;           uint irr_texture;                         ; Offset:   28
;           uint rad_texture;                         ; Offset:   32
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    36
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 23, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 16, 17, 18, 20, 21, 22 }
;   output 1 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 16, 17, 18, 20, 21, 22 }
;   output 2 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 16, 17, 18, 20, 21, 22 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }

define void @draw_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 36 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %13 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %14 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %15 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %16 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %17 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.i32 %17, 0
  %19 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %18, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %20 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %19, %dx.types.ResourceProperties { i32 13, i32 708 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %21 = extractvalue %dx.types.CBufRet.i32 %17, 2
  %22 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %21, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %23 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %22, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %24 = extractvalue %dx.types.CBufRet.i32 %17, 3
  %25 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %24, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %26 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %25, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %27 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.i32 %27, 0
  %29 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %28, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %30 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %29, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %31 = extractvalue %dx.types.CBufRet.i32 %27, 1
  %32 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %31, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %33 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %32, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %34 = extractvalue %dx.types.CBufRet.i32 %27, 2
  %35 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %34, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %36 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %35, %dx.types.ResourceProperties { i32 2, i32 521 })  ; AnnotateHandle(res,props)  resource: Texture2D<2xF32>
  %37 = extractvalue %dx.types.CBufRet.i32 %27, 3
  %38 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %37, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %39 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %38, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %40 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %41 = extractvalue %dx.types.CBufRet.i32 %40, 0
  %42 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %41, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %43 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %42, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %44 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %23, %dx.types.Handle %33, float %12, float %13, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %45 = extractvalue %dx.types.ResRet.f32 %44, 0
  %46 = extractvalue %dx.types.ResRet.f32 %44, 1
  %47 = extractvalue %dx.types.ResRet.f32 %44, 2
  %48 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %26, %dx.types.Handle %33, float %12, float %13, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %49 = extractvalue %dx.types.ResRet.f32 %48, 0
  %50 = extractvalue %dx.types.ResRet.f32 %48, 1
  %51 = extractvalue %dx.types.ResRet.f32 %48, 2
  %52 = fmul fast float %49, 2.000000e+00
  %53 = fmul fast float %50, 2.000000e+00
  %54 = fmul fast float %51, 2.000000e+00
  %55 = fadd fast float %52, -1.000000e+00
  %56 = fadd fast float %53, -1.000000e+00
  %57 = fadd fast float %54, -1.000000e+00
  %58 = call float @dx.op.dot3.f32(i32 55, float %55, float %56, float %57, float %55, float %56, float %57)  ; Dot3(ax,ay,az,bx,by,bz)
  %59 = call float @dx.op.unary.f32(i32 25, float %58)  ; Rsqrt(value)
  %60 = fmul fast float %55, %59
  %61 = fmul fast float %56, %59
  %62 = fmul fast float %57, %59
  %63 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %30, %dx.types.Handle %33, float %12, float %13, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %64 = extractvalue %dx.types.ResRet.f32 %63, 2
  %65 = extractvalue %dx.types.ResRet.f32 %63, 1
  %66 = fadd fast float %45, 0xBFA47AE140000000
  %67 = fadd fast float %46, 0xBFA47AE140000000
  %68 = fadd fast float %47, 0xBFA47AE140000000
  %69 = fmul fast float %64, %66
  %70 = fmul fast float %64, %67
  %71 = fmul fast float %64, %68
  %72 = fadd fast float %69, 0x3FA47AE140000000
  %73 = fadd fast float %70, 0x3FA47AE140000000
  %74 = fadd fast float %71, 0x3FA47AE140000000
  %75 = fmul fast float %60, %9
  %76 = call float @dx.op.tertiary.f32(i32 46, float %61, float %6, float %75)  ; FMad(a,b,c)
  %77 = call float @dx.op.tertiary.f32(i32 46, float %62, float %14, float %76)  ; FMad(a,b,c)
  %78 = fmul fast float %60, %10
  %79 = call float @dx.op.tertiary.f32(i32 46, float %61, float %7, float %78)  ; FMad(a,b,c)
  %80 = call float @dx.op.tertiary.f32(i32 46, float %62, float %15, float %79)  ; FMad(a,b,c)
  %81 = fmul fast float %60, %11
  %82 = call float @dx.op.tertiary.f32(i32 46, float %61, float %8, float %81)  ; FMad(a,b,c)
  %83 = call float @dx.op.tertiary.f32(i32 46, float %62, float %16, float %82)  ; FMad(a,b,c)
  %84 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %20, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %85 = extractvalue %dx.types.CBufRet.f32 %84, 1
  %86 = extractvalue %dx.types.CBufRet.f32 %84, 2
  %87 = extractvalue %dx.types.CBufRet.f32 %84, 3
  %88 = fsub fast float %85, %3
  %89 = fsub fast float %86, %4
  %90 = fsub fast float %87, %5
  %91 = call float @dx.op.dot3.f32(i32 55, float %88, float %89, float %90, float %88, float %89, float %90)  ; Dot3(ax,ay,az,bx,by,bz)
  %92 = call float @dx.op.unary.f32(i32 25, float %91)  ; Rsqrt(value)
  %93 = fmul fast float %88, %92
  %94 = fmul fast float %89, %92
  %95 = fmul fast float %90, %92
  %96 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %20, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %97 = extractvalue %dx.types.CBufRet.f32 %96, 0
  %98 = extractvalue %dx.types.CBufRet.f32 %96, 1
  %99 = extractvalue %dx.types.CBufRet.f32 %96, 2
  %100 = call float @dx.op.dot3.f32(i32 55, float %77, float %80, float %83, float %97, float %98, float %99)  ; Dot3(ax,ay,az,bx,by,bz)
  %101 = call float @dx.op.dot3.f32(i32 55, float %77, float %80, float %83, float %93, float %94, float %95)  ; Dot3(ax,ay,az,bx,by,bz)
  %102 = fadd fast float %97, %93
  %103 = fadd fast float %98, %94
  %104 = fadd fast float %99, %95
  %105 = call float @dx.op.dot3.f32(i32 55, float %102, float %103, float %104, float %102, float %103, float %104)  ; Dot3(ax,ay,az,bx,by,bz)
  %106 = call float @dx.op.unary.f32(i32 25, float %105)  ; Rsqrt(value)
  %107 = fmul fast float %106, %102
  %108 = fmul fast float %106, %103
  %109 = fmul fast float %106, %104
  %110 = fcmp fast ole float %101, 0.000000e+00
  %111 = fcmp fast ole float %100, 0.000000e+00
  %112 = call float @dx.op.binary.f32(i32 35, float %100, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %113 = call float @dx.op.binary.f32(i32 36, float %112, float 1.000000e+00)  ; FMin(a,b)
  %114 = call float @dx.op.binary.f32(i32 35, float %101, float 0x3EE4F8B580000000)  ; FMax(a,b)
  %115 = call float @dx.op.binary.f32(i32 36, float %114, float 1.000000e+00)  ; FMin(a,b)
  %116 = call float @dx.op.dot3.f32(i32 55, float %97, float %98, float %99, float %107, float %108, float %109)  ; Dot3(ax,ay,az,bx,by,bz)
  %117 = call float @dx.op.unary.f32(i32 7, float %116)  ; Saturate(value)
  %118 = call float @dx.op.dot3.f32(i32 55, float %77, float %80, float %83, float %107, float %108, float %109)  ; Dot3(ax,ay,az,bx,by,bz)
  %119 = call float @dx.op.unary.f32(i32 7, float %118)  ; Saturate(value)
  %120 = fsub fast float 1.000000e+00, %64
  %121 = fmul fast float %65, %65
  %122 = fmul fast float %121, %121
  %123 = call float @dx.op.dot3.f32(i32 55, float %72, float %73, float %74, float 0x3FCB367A00000000, float 0x3FE6E2EB20000000, float 0x3FB27BB300000000)  ; Dot3(ax,ay,az,bx,by,bz)
  %124 = fmul fast float %123, 2.500000e+01
  %125 = call float @dx.op.binary.f32(i32 36, float 1.000000e+00, float %124)  ; FMin(a,b)
  %126 = fsub fast float %125, %72
  %127 = fsub fast float %125, %73
  %128 = fsub fast float %125, %74
  %129 = fsub fast float 1.000000e+00, %117
  %130 = call float @dx.op.unary.f32(i32 23, float %129)  ; Log(value)
  %131 = fmul fast float %130, 5.000000e+00
  %132 = call float @dx.op.unary.f32(i32 21, float %131)  ; Exp(value)
  %133 = fmul fast float %132, %126
  %134 = fmul fast float %132, %127
  %135 = fmul fast float %132, %128
  %136 = fadd fast float %133, %72
  %137 = fadd fast float %134, %73
  %138 = fadd fast float %135, %74
  %139 = or i1 %111, %110
  br i1 %139, label %202, label %140

; <label>:140                                     ; preds = %0
  %141 = fmul fast float %120, %47
  %142 = fmul fast float %120, %46
  %143 = fmul fast float %120, %45
  %144 = call float @dx.op.binary.f32(i32 35, float 0x3EE4F8B580000000, float %122)  ; FMax(a,b)
  %145 = fadd fast float %144, -1.000000e+00
  %146 = fmul fast float %119, %119
  %147 = fmul fast float %146, %145
  %148 = fadd fast float %147, 1.000000e+00
  %149 = fmul fast float %148, %148
  %150 = fmul fast float %149, 0x400921FB60000000
  %151 = fdiv fast float %144, %150
  %152 = fmul fast float %113, %122
  %153 = fsub fast float %113, %152
  %154 = fmul fast float %153, %113
  %155 = fadd fast float %154, %122
  %156 = call float @dx.op.unary.f32(i32 24, float %155)  ; Sqrt(value)
  %157 = fmul fast float %156, %115
  %158 = fmul fast float %115, %122
  %159 = fsub fast float %115, %158
  %160 = fmul fast float %159, %115
  %161 = fadd fast float %160, %122
  %162 = call float @dx.op.unary.f32(i32 24, float %161)  ; Sqrt(value)
  %163 = fmul fast float %162, %113
  %164 = fadd fast float %163, %157
  %165 = fdiv fast float 5.000000e-01, %164
  %166 = fmul fast float %151, %113
  %167 = fmul fast float %166, %165
  %168 = fmul fast float %167, %136
  %169 = fmul fast float %167, %137
  %170 = fmul fast float %167, %138
  %171 = fmul fast float %65, 2.000000e+00
  %172 = fmul fast float %117, %117
  %173 = fmul fast float %172, %171
  %174 = fadd fast float %173, -5.000000e-01
  %175 = fsub fast float 1.000000e+00, %113
  %176 = call float @dx.op.unary.f32(i32 23, float %175)  ; Log(value)
  %177 = fmul fast float %176, 5.000000e+00
  %178 = call float @dx.op.unary.f32(i32 21, float %177)  ; Exp(value)
  %179 = fmul fast float %178, %174
  %180 = fadd fast float %179, 1.000000e+00
  %181 = fsub fast float 1.000000e+00, %115
  %182 = call float @dx.op.unary.f32(i32 23, float %181)  ; Log(value)
  %183 = fmul fast float %182, 5.000000e+00
  %184 = call float @dx.op.unary.f32(i32 21, float %183)  ; Exp(value)
  %185 = fmul fast float %184, %174
  %186 = fadd fast float %185, 1.000000e+00
  %187 = fmul fast float %113, 0x3FD45F3060000000
  %188 = fmul fast float %187, %180
  %189 = fmul fast float %188, %186
  %190 = fsub fast float 1.000000e+00, %136
  %191 = fsub fast float 1.000000e+00, %137
  %192 = fsub fast float 1.000000e+00, %138
  %193 = fmul fast float %143, %190
  %194 = fmul fast float %193, %189
  %195 = fmul fast float %142, %191
  %196 = fmul fast float %195, %189
  %197 = fmul fast float %141, %192
  %198 = fmul fast float %197, %189
  %199 = fadd fast float %194, %168
  %200 = fadd fast float %196, %169
  %201 = fadd fast float %198, %170
  br label %202

; <label>:202                                     ; preds = %140, %0
  %203 = phi float [ %199, %140 ], [ 0.000000e+00, %0 ]
  %204 = phi float [ %200, %140 ], [ 0.000000e+00, %0 ]
  %205 = phi float [ %201, %140 ], [ 0.000000e+00, %0 ]
  %206 = fsub fast float -0.000000e+00, %93
  %207 = fsub fast float -0.000000e+00, %94
  %208 = fsub fast float -0.000000e+00, %95
  %209 = call float @dx.op.dot3.f32(i32 55, float %206, float %207, float %208, float %77, float %80, float %83)  ; Dot3(ax,ay,az,bx,by,bz)
  %210 = fmul fast float %209, 2.000000e+00
  %211 = fmul fast float %210, %77
  %212 = fmul fast float %210, %80
  %213 = fmul fast float %210, %83
  %214 = fsub fast float %206, %211
  %215 = fsub fast float %207, %212
  %216 = fsub fast float %208, %213
  %217 = call float @dx.op.dot3.f32(i32 55, float %77, float %80, float %83, float %214, float %215, float %216)  ; Dot3(ax,ay,az,bx,by,bz)
  %218 = call float @dx.op.unary.f32(i32 7, float %217)  ; Saturate(value)
  %219 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %20, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %220 = extractvalue %dx.types.CBufRet.i32 %219, 1
  %221 = add i32 %220, -1
  %222 = uitofp i32 %221 to float
  %223 = fmul fast float %222, %65
  %224 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %36, %dx.types.Handle %33, float %218, float %65, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %225 = extractvalue %dx.types.ResRet.f32 %224, 0
  %226 = extractvalue %dx.types.ResRet.f32 %224, 1
  %227 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %39, %dx.types.Handle %33, float %77, float %80, float %83, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %228 = extractvalue %dx.types.ResRet.f32 %227, 0
  %229 = extractvalue %dx.types.ResRet.f32 %227, 1
  %230 = extractvalue %dx.types.ResRet.f32 %227, 2
  %231 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %43, %dx.types.Handle %33, float %214, float %215, float %216, float undef, i32 undef, i32 undef, i32 undef, float %223)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %232 = extractvalue %dx.types.ResRet.f32 %231, 0
  %233 = extractvalue %dx.types.ResRet.f32 %231, 1
  %234 = extractvalue %dx.types.ResRet.f32 %231, 2
  %235 = fmul fast float %228, %45
  %236 = fmul fast float %229, %46
  %237 = fmul fast float %230, %47
  %238 = fmul fast float %225, %72
  %239 = fmul fast float %225, %73
  %240 = fmul fast float %225, %74
  %241 = fadd fast float %238, %226
  %242 = fadd fast float %239, %226
  %243 = fadd fast float %240, %226
  %244 = fmul fast float %232, %241
  %245 = fmul fast float %233, %242
  %246 = fmul fast float %234, %243
  %247 = call float @dx.op.unary.f32(i32 7, float %100)  ; Saturate(value)
  %248 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %20, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %249 = extractvalue %dx.types.CBufRet.f32 %248, 3
  %250 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %20, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %251 = extractvalue %dx.types.CBufRet.f32 %250, 0
  %252 = fsub fast float 1.000000e+00, %249
  %253 = fmul fast float %252, %247
  %254 = fadd fast float %253, %249
  %255 = fmul fast float %254, %251
  %256 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %20, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %257 = extractvalue %dx.types.CBufRet.i32 %256, 0
  switch i32 %257, label %336 [
    i32 0, label %258
    i32 1, label %262
    i32 2, label %263
    i32 3, label %264
    i32 4, label %265
    i32 5, label %266
    i32 6, label %267
    i32 7, label %271
    i32 8, label %272
    i32 9, label %278
    i32 10, label %285
    i32 11, label %286
    i32 12, label %293
    i32 13, label %294
    i32 14, label %295
    i32 15, label %298
    i32 16, label %305
    i32 17, label %312
    i32 18, label %319
    i32 19, label %322
    i32 20, label %329
  ]

; <label>:258                                     ; preds = %202
  %259 = fadd fast float %244, %235
  %260 = fadd fast float %245, %236
  %261 = fadd fast float %246, %237
  br label %336

; <label>:262                                     ; preds = %202
  br label %336

; <label>:263                                     ; preds = %202
  br label %336

; <label>:264                                     ; preds = %202
  br label %336

; <label>:265                                     ; preds = %202
  br label %336

; <label>:266                                     ; preds = %202
  br label %336

; <label>:267                                     ; preds = %202
  %268 = fmul fast float %255, %203
  %269 = fmul fast float %255, %204
  %270 = fmul fast float %255, %205
  br label %336

; <label>:271                                     ; preds = %202
  br label %336

; <label>:272                                     ; preds = %202
  %273 = call float @dx.op.dot3.f32(i32 55, float %14, float %15, float %16, float %97, float %98, float %99)  ; Dot3(ax,ay,az,bx,by,bz)
  %274 = call float @dx.op.unary.f32(i32 7, float %273)  ; Saturate(value)
  %275 = fmul fast float %252, %274
  %276 = fadd fast float %275, %249
  %277 = fmul fast float %276, %251
  br label %336

; <label>:278                                     ; preds = %202
  %279 = fadd fast float %77, 1.000000e+00
  %280 = fadd fast float %80, 1.000000e+00
  %281 = fadd fast float %83, 1.000000e+00
  %282 = fmul fast float %279, 5.000000e-01
  %283 = fmul fast float %280, 5.000000e-01
  %284 = fmul fast float %281, 5.000000e-01
  br label %336

; <label>:285                                     ; preds = %202
  br label %336

; <label>:286                                     ; preds = %202
  %287 = fadd fast float %60, 1.000000e+00
  %288 = fadd fast float %61, 1.000000e+00
  %289 = fadd fast float %62, 1.000000e+00
  %290 = fmul fast float %287, 5.000000e-01
  %291 = fmul fast float %288, 5.000000e-01
  %292 = fmul fast float %289, 5.000000e-01
  br label %336

; <label>:293                                     ; preds = %202
  br label %336

; <label>:294                                     ; preds = %202
  br label %336

; <label>:295                                     ; preds = %202
  %296 = call float @dx.op.unary.f32(i32 22, float %12)  ; Frc(value)
  %297 = call float @dx.op.unary.f32(i32 22, float %13)  ; Frc(value)
  br label %336

; <label>:298                                     ; preds = %202
  %299 = fadd fast float %14, 1.000000e+00
  %300 = fadd fast float %15, 1.000000e+00
  %301 = fadd fast float %16, 1.000000e+00
  %302 = fmul fast float %299, 5.000000e-01
  %303 = fmul fast float %300, 5.000000e-01
  %304 = fmul fast float %301, 5.000000e-01
  br label %336

; <label>:305                                     ; preds = %202
  %306 = fadd fast float %9, 1.000000e+00
  %307 = fadd fast float %10, 1.000000e+00
  %308 = fadd fast float %11, 1.000000e+00
  %309 = fmul fast float %306, 5.000000e-01
  %310 = fmul fast float %307, 5.000000e-01
  %311 = fmul fast float %308, 5.000000e-01
  br label %336

; <label>:312                                     ; preds = %202
  %313 = fadd fast float %6, 1.000000e+00
  %314 = fadd fast float %7, 1.000000e+00
  %315 = fadd fast float %8, 1.000000e+00
  %316 = fmul fast float %313, 5.000000e-01
  %317 = fmul fast float %314, 5.000000e-01
  %318 = fmul fast float %315, 5.000000e-01
  br label %336

; <label>:319                                     ; preds = %202
  %320 = call float @dx.op.calculateLOD.f32(i32 81, %dx.types.Handle %23, %dx.types.Handle %33, float %12, float %13, float undef, i1 true)  ; CalculateLOD(handle,sampler,coord0,coord1,coord2,clamped)
  %321 = call float @dx.op.unary.f32(i32 22, float %320)  ; Frc(value)
  br label %336

; <label>:322                                     ; preds = %202
  %323 = fadd fast float %93, 1.000000e+00
  %324 = fadd fast float %94, 1.000000e+00
  %325 = fadd fast float %95, 1.000000e+00
  %326 = fmul fast float %323, 5.000000e-01
  %327 = fmul fast float %324, 5.000000e-01
  %328 = fmul fast float %325, 5.000000e-01
  br label %336

; <label>:329                                     ; preds = %202
  %330 = fadd fast float %97, 1.000000e+00
  %331 = fadd fast float %98, 1.000000e+00
  %332 = fadd fast float %99, 1.000000e+00
  %333 = fmul fast float %330, 5.000000e-01
  %334 = fmul fast float %331, 5.000000e-01
  %335 = fmul fast float %332, 5.000000e-01
  br label %336

; <label>:336                                     ; preds = %329, %322, %319, %312, %305, %298, %295, %294, %293, %286, %285, %278, %272, %271, %267, %266, %265, %264, %263, %262, %258, %202
  %337 = phi float [ undef, %202 ], [ %333, %329 ], [ %326, %322 ], [ %321, %319 ], [ %316, %312 ], [ %309, %305 ], [ %302, %298 ], [ %296, %295 ], [ %65, %294 ], [ %64, %293 ], [ %290, %286 ], [ %45, %285 ], [ %282, %278 ], [ %277, %272 ], [ %203, %271 ], [ %268, %267 ], [ %232, %266 ], [ %228, %265 ], [ %225, %264 ], [ %244, %263 ], [ %235, %262 ], [ %259, %258 ]
  %338 = phi float [ undef, %202 ], [ %334, %329 ], [ %327, %322 ], [ %321, %319 ], [ %317, %312 ], [ %310, %305 ], [ %303, %298 ], [ %297, %295 ], [ %65, %294 ], [ %64, %293 ], [ %291, %286 ], [ %46, %285 ], [ %283, %278 ], [ %277, %272 ], [ %204, %271 ], [ %269, %267 ], [ %233, %266 ], [ %229, %265 ], [ %226, %264 ], [ %245, %263 ], [ %236, %262 ], [ %260, %258 ]
  %339 = phi float [ undef, %202 ], [ %335, %329 ], [ %328, %322 ], [ %321, %319 ], [ %318, %312 ], [ %311, %305 ], [ %304, %298 ], [ 0.000000e+00, %295 ], [ %65, %294 ], [ %64, %293 ], [ %292, %286 ], [ %47, %285 ], [ %284, %278 ], [ %277, %272 ], [ %205, %271 ], [ %270, %267 ], [ %234, %266 ], [ %230, %265 ], [ 0.000000e+00, %264 ], [ %246, %263 ], [ %237, %262 ], [ %261, %258 ]
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %337)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %338)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %339)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare float @dx.op.calculateLOD.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, i1) #2

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 36, null}
!6 = !{[25 x i32] [i32 23, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @draw_ps, !"draw_ps", !8, !3, !26}
!8 = !{!9, !23, null}
!9 = !{!10, !12, !14, !17, !19, !21}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, !16}
!15 = !{i32 1}
!16 = !{i32 3, i32 3}
!17 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !18, i8 2, i32 1, i8 4, i32 3, i8 0, !13}
!18 = !{i32 2}
!19 = !{i32 4, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 3, i32 4, i8 0, !13}
!20 = !{i32 3}
!21 = !{i32 5, !"ATTRIBUTE", i8 9, i8 0, !22, i8 2, i32 1, i8 3, i32 5, i8 0, !13}
!22 = !{i32 4}
!23 = !{!24}
!24 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !25}
!25 = !{i32 3, i32 15}
!26 = !{i32 0, i64 3229614336}
*/
auto Shaders::crate_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(50112, 8012);
}

// shader_hash: da96164371dd2400e55d53254b743477
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 55
// float_instruction_count: 8
// texture_load_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
;
; shader debug name: da96164371dd2400e55d53254b743477.pdb
; shader hash: da96164371dd2400e55d53254b743477
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;           uint shadow_texture;                      ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @shadow_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 580 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %16 = extractvalue %dx.types.CBufRet.f32 %15, 0
  %17 = extractvalue %dx.types.CBufRet.f32 %15, 1
  %18 = extractvalue %dx.types.CBufRet.f32 %15, 2
  %19 = extractvalue %dx.types.CBufRet.f32 %15, 3
  %20 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %21 = extractvalue %dx.types.CBufRet.f32 %20, 0
  %22 = extractvalue %dx.types.CBufRet.f32 %20, 1
  %23 = extractvalue %dx.types.CBufRet.f32 %20, 2
  %24 = extractvalue %dx.types.CBufRet.f32 %20, 3
  %25 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %26 = extractvalue %dx.types.CBufRet.f32 %25, 0
  %27 = extractvalue %dx.types.CBufRet.f32 %25, 1
  %28 = extractvalue %dx.types.CBufRet.f32 %25, 2
  %29 = extractvalue %dx.types.CBufRet.f32 %25, 3
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 2
  %34 = extractvalue %dx.types.CBufRet.f32 %30, 3
  %35 = fmul fast float %16, %12
  %36 = call float @dx.op.tertiary.f32(i32 46, float %21, float %13, float %35)  ; FMad(a,b,c)
  %37 = call float @dx.op.tertiary.f32(i32 46, float %26, float %14, float %36)  ; FMad(a,b,c)
  %38 = fadd fast float %37, %31
  %39 = fmul fast float %17, %12
  %40 = call float @dx.op.tertiary.f32(i32 46, float %22, float %13, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %27, float %14, float %40)  ; FMad(a,b,c)
  %42 = fadd fast float %41, %32
  %43 = fmul fast float %18, %12
  %44 = call float @dx.op.tertiary.f32(i32 46, float %23, float %13, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %28, float %14, float %44)  ; FMad(a,b,c)
  %46 = fadd fast float %45, %33
  %47 = fmul fast float %19, %12
  %48 = call float @dx.op.tertiary.f32(i32 46, float %24, float %13, float %47)  ; FMad(a,b,c)
  %49 = call float @dx.op.tertiary.f32(i32 46, float %29, float %14, float %48)  ; FMad(a,b,c)
  %50 = fadd fast float %49, %34
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %38)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %42)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %46)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %50)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[7 x i32] [i32 5, i32 4, i32 15, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @shadow_vs, !"shadow_vs", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 1082130688}
*/
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(58124, 4672);
}

// shader_hash: 02836609226e6e4020c070d23176453a
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 107
// float_instruction_count: 16
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyzw        3     NONE   float   xyzw
;
; shader debug name: 02836609226e6e4020c070d23176453a.pdb
; shader hash: 02836609226e6e4020c070d23176453a
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;           uint shadow_texture;                      ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 16
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;   output 15 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 580 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %27 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.f32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.f32 %27, 1
  %30 = extractvalue %dx.types.CBufRet.f32 %27, 2
  %31 = extractvalue %dx.types.CBufRet.f32 %27, 3
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 0
  %34 = extractvalue %dx.types.CBufRet.f32 %32, 1
  %35 = extractvalue %dx.types.CBufRet.f32 %32, 2
  %36 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = extractvalue %dx.types.CBufRet.f32 %37, 1
  %40 = extractvalue %dx.types.CBufRet.f32 %37, 2
  %41 = extractvalue %dx.types.CBufRet.f32 %37, 3
  %42 = fmul fast float %23, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %38
  %46 = fmul fast float %24, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %34, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %39
  %50 = fmul fast float %25, %12
  %51 = call float @dx.op.tertiary.f32(i32 46, float %30, float %13, float %50)  ; FMad(a,b,c)
  %52 = call float @dx.op.tertiary.f32(i32 46, float %35, float %14, float %51)  ; FMad(a,b,c)
  %53 = fadd fast float %52, %40
  %54 = fmul fast float %26, %12
  %55 = call float @dx.op.tertiary.f32(i32 46, float %31, float %13, float %54)  ; FMad(a,b,c)
  %56 = call float @dx.op.tertiary.f32(i32 46, float %36, float %14, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %41
  %58 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %59 = extractvalue %dx.types.CBufRet.f32 %58, 0
  %60 = extractvalue %dx.types.CBufRet.f32 %58, 1
  %61 = extractvalue %dx.types.CBufRet.f32 %58, 2
  %62 = extractvalue %dx.types.CBufRet.f32 %58, 3
  %63 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %64 = extractvalue %dx.types.CBufRet.f32 %63, 0
  %65 = extractvalue %dx.types.CBufRet.f32 %63, 1
  %66 = extractvalue %dx.types.CBufRet.f32 %63, 2
  %67 = extractvalue %dx.types.CBufRet.f32 %63, 3
  %68 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %69 = extractvalue %dx.types.CBufRet.f32 %68, 0
  %70 = extractvalue %dx.types.CBufRet.f32 %68, 1
  %71 = extractvalue %dx.types.CBufRet.f32 %68, 2
  %72 = extractvalue %dx.types.CBufRet.f32 %68, 3
  %73 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %74 = extractvalue %dx.types.CBufRet.f32 %73, 0
  %75 = extractvalue %dx.types.CBufRet.f32 %73, 1
  %76 = extractvalue %dx.types.CBufRet.f32 %73, 2
  %77 = extractvalue %dx.types.CBufRet.f32 %73, 3
  %78 = fmul fast float %59, %12
  %79 = call float @dx.op.tertiary.f32(i32 46, float %64, float %13, float %78)  ; FMad(a,b,c)
  %80 = call float @dx.op.tertiary.f32(i32 46, float %69, float %14, float %79)  ; FMad(a,b,c)
  %81 = fadd fast float %80, %74
  %82 = fmul fast float %60, %12
  %83 = call float @dx.op.tertiary.f32(i32 46, float %65, float %13, float %82)  ; FMad(a,b,c)
  %84 = call float @dx.op.tertiary.f32(i32 46, float %70, float %14, float %83)  ; FMad(a,b,c)
  %85 = fadd fast float %84, %75
  %86 = fmul fast float %61, %12
  %87 = call float @dx.op.tertiary.f32(i32 46, float %66, float %13, float %86)  ; FMad(a,b,c)
  %88 = call float @dx.op.tertiary.f32(i32 46, float %71, float %14, float %87)  ; FMad(a,b,c)
  %89 = fadd fast float %88, %76
  %90 = fmul fast float %62, %12
  %91 = call float @dx.op.tertiary.f32(i32 46, float %67, float %13, float %90)  ; FMad(a,b,c)
  %92 = call float @dx.op.tertiary.f32(i32 46, float %72, float %14, float %91)  ; FMad(a,b,c)
  %93 = fadd fast float %92, %77
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %81)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %85)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %89)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 3, float %93)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[7 x i32] [i32 5, i32 16, i32 62335, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 4, i32 3, i8 0, !16}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(62796, 5472);
}

// shader_hash: dd367431c37c158d4eee8042d74ffd2f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 58
// float_instruction_count: 17
// texture_normal_instructions: 1
// texture_comp_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyzw        3     NONE   float   xyzw
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: dd367431c37c158d4eee8042d74ffd2f.pdb
; shader hash: dd367431c37c158d4eee8042d74ffd2f
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;           uint shadow_texture;                      ; Offset:   12
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 16, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 15 }
;   output 1 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 15 }
;   output 2 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14, 15 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32 }

define void @draw_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %13 = extractvalue %dx.types.CBufRet.i32 %12, 0
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 13, i32 580 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %16 = extractvalue %dx.types.CBufRet.i32 %12, 2
  %17 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %16, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %18 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %17, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %19 = extractvalue %dx.types.CBufRet.i32 %12, 3
  %20 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %19, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %21 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %20, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %22 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %23 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %22, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %24 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 6, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %25 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %24, %dx.types.ResourceProperties { i32 32782, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerComparisonState
  %26 = fdiv fast float %3, %6
  %27 = fdiv fast float %4, %6
  %28 = fdiv fast float %5, %6
  %29 = fmul fast float %26, 5.000000e-01
  %30 = fmul fast float %27, 5.000000e-01
  %31 = fadd fast float %29, 5.000000e-01
  %32 = fsub fast float 5.000000e-01, %30
  %33 = fadd fast float %28, 0xBF50624DE0000000
  %34 = call %dx.types.ResRet.f32 @dx.op.sampleCmpLevelZero.f32(i32 65, %dx.types.Handle %21, %dx.types.Handle %25, float %31, float %32, float undef, float undef, i32 0, i32 0, i32 undef, float %33)  ; SampleCmpLevelZero(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,compareValue)
  %35 = extractvalue %dx.types.ResRet.f32 %34, 0
  %36 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %18, %dx.types.Handle %23, float %7, float %8, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %37 = extractvalue %dx.types.ResRet.f32 %36, 0
  %38 = extractvalue %dx.types.ResRet.f32 %36, 1
  %39 = extractvalue %dx.types.ResRet.f32 %36, 2
  %40 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %15, i32 8)  ; CBufferLoadLegacy(handle,regIndex)
  %41 = extractvalue %dx.types.CBufRet.f32 %40, 0
  %42 = extractvalue %dx.types.CBufRet.f32 %40, 1
  %43 = extractvalue %dx.types.CBufRet.f32 %40, 2
  %44 = call float @dx.op.dot3.f32(i32 55, float %9, float %10, float %11, float %41, float %42, float %43)  ; Dot3(ax,ay,az,bx,by,bz)
  %45 = call float @dx.op.unary.f32(i32 7, float %44)  ; Saturate(value)
  %46 = fmul fast float %45, %35
  %47 = extractvalue %dx.types.CBufRet.f32 %40, 3
  %48 = fsub fast float 1.000000e+00, %47
  %49 = fmul fast float %46, %48
  %50 = fadd fast float %49, %47
  %51 = fmul fast float %50, %37
  %52 = fmul fast float %50, %38
  %53 = fmul fast float %50, %39
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %51)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %52)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleCmpLevelZero.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[18 x i32] [i32 16, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 0, i32 0, i32 7, i32 7, i32 7, i32 7]}
!7 = !{void ()* @draw_ps, !"draw_ps", !8, !3, !22}
!8 = !{!9, !20, null}
!9 = !{!10, !12, !14, !17}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, !16}
!15 = !{i32 1}
!16 = !{i32 3, i32 3}
!17 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !18, i8 2, i32 1, i8 4, i32 3, i8 0, !19}
!18 = !{i32 2}
!19 = !{i32 3, i32 15}
!20 = !{!21}
!21 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !19}
!22 = !{i32 0, i64 3229614336}
*/
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(68268, 5236);
}

// shader_hash: 594b8b4c846ab4dced1159f304ce8720
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 23
// float_instruction_count: 3
// texture_load_instructions: 1
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: 594b8b4c846ab4dced1159f304ce8720.pdb
; shader hash: 594b8b4c846ab4dced1159f304ce8720
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(128,1,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint particles;                           ; Offset:    4
;           uint vertices;                            ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @sim_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 612 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 4620, i32 12 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=12>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 8)  ; CBufferLoadLegacy(handle,regIndex)
  %16 = extractvalue %dx.types.CBufRet.f32 %15, 1
  %17 = extractvalue %dx.types.CBufRet.f32 %15, 0
  %18 = fmul fast float %17, %16
  %19 = fsub fast float %13, %18
  %20 = fcmp fast olt float %19, -1.000000e+00
  %21 = select i1 %20, float 1.000000e+00, float %19
  call void @dx.op.rawBufferStore.f32(i32 140, %dx.types.Handle %10, i32 %3, i32 0, float %12, float %21, float %14, float undef, i8 7, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #1

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.f32(i32, %dx.types.Handle, i32, i32, float, float, float, float, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{void ()* @sim_cs, !"sim_cs", null, !3, !7}
!7 = !{i32 0, i64 9672065280, i32 4, !8}
!8 = !{i32 128, i32 1, i32 1}
*/
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(73504, 3752);
}

// shader_hash: 5c734c27c8b9608ce06b8b204a05def0
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 92
// float_instruction_count: 14
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: 5c734c27c8b9608ce06b8b204a05def0.pdb
; shader hash: 5c734c27c8b9608ce06b8b204a05def0
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint particles;                           ; Offset:    4
;           uint vertices;                            ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 4 }
;   output 1 depends on inputs: { 0, 4 }
;   output 2 depends on inputs: { 0, 4 }
;   output 3 depends on inputs: { 0, 4 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 612 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 524, i32 20 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=20>
  %12 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 524, i32 12 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=12>
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %4, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %4, i32 12, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 2
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 2
  %34 = fmul fast float %23, %16
  %35 = call float @dx.op.tertiary.f32(i32 46, float %27, float %17, float %34)  ; FMad(a,b,c)
  %36 = call float @dx.op.tertiary.f32(i32 46, float %31, float %18, float %35)  ; FMad(a,b,c)
  %37 = fmul fast float %24, %16
  %38 = call float @dx.op.tertiary.f32(i32 46, float %28, float %17, float %37)  ; FMad(a,b,c)
  %39 = call float @dx.op.tertiary.f32(i32 46, float %32, float %18, float %38)  ; FMad(a,b,c)
  %40 = fmul fast float %25, %16
  %41 = call float @dx.op.tertiary.f32(i32 46, float %29, float %17, float %40)  ; FMad(a,b,c)
  %42 = call float @dx.op.tertiary.f32(i32 46, float %33, float %18, float %41)  ; FMad(a,b,c)
  %43 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %44 = extractvalue %dx.types.ResRet.f32 %43, 0
  %45 = extractvalue %dx.types.ResRet.f32 %43, 1
  %46 = extractvalue %dx.types.ResRet.f32 %43, 2
  %47 = fadd fast float %44, %36
  %48 = fadd fast float %45, %39
  %49 = fadd fast float %46, %42
  %50 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %51 = extractvalue %dx.types.CBufRet.f32 %50, 0
  %52 = extractvalue %dx.types.CBufRet.f32 %50, 1
  %53 = extractvalue %dx.types.CBufRet.f32 %50, 2
  %54 = extractvalue %dx.types.CBufRet.f32 %50, 3
  %55 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %56 = extractvalue %dx.types.CBufRet.f32 %55, 0
  %57 = extractvalue %dx.types.CBufRet.f32 %55, 1
  %58 = extractvalue %dx.types.CBufRet.f32 %55, 2
  %59 = extractvalue %dx.types.CBufRet.f32 %55, 3
  %60 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %61 = extractvalue %dx.types.CBufRet.f32 %60, 0
  %62 = extractvalue %dx.types.CBufRet.f32 %60, 1
  %63 = extractvalue %dx.types.CBufRet.f32 %60, 2
  %64 = extractvalue %dx.types.CBufRet.f32 %60, 3
  %65 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %66 = extractvalue %dx.types.CBufRet.f32 %65, 0
  %67 = extractvalue %dx.types.CBufRet.f32 %65, 1
  %68 = extractvalue %dx.types.CBufRet.f32 %65, 2
  %69 = extractvalue %dx.types.CBufRet.f32 %65, 3
  %70 = fmul fast float %51, %47
  %71 = call float @dx.op.tertiary.f32(i32 46, float %56, float %48, float %70)  ; FMad(a,b,c)
  %72 = call float @dx.op.tertiary.f32(i32 46, float %61, float %49, float %71)  ; FMad(a,b,c)
  %73 = fadd fast float %72, %66
  %74 = fmul fast float %52, %47
  %75 = call float @dx.op.tertiary.f32(i32 46, float %57, float %48, float %74)  ; FMad(a,b,c)
  %76 = call float @dx.op.tertiary.f32(i32 46, float %62, float %49, float %75)  ; FMad(a,b,c)
  %77 = fadd fast float %76, %67
  %78 = fmul fast float %53, %47
  %79 = call float @dx.op.tertiary.f32(i32 46, float %58, float %48, float %78)  ; FMad(a,b,c)
  %80 = call float @dx.op.tertiary.f32(i32 46, float %63, float %49, float %79)  ; FMad(a,b,c)
  %81 = fadd fast float %80, %68
  %82 = fmul fast float %54, %47
  %83 = call float @dx.op.tertiary.f32(i32 46, float %59, float %48, float %82)  ; FMad(a,b,c)
  %84 = call float @dx.op.tertiary.f32(i32 46, float %64, float %49, float %83)  ; FMad(a,b,c)
  %85 = fadd fast float %84, %69
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %73)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %77)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %81)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %85)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[7 x i32] [i32 5, i32 6, i32 63, i32 0, i32 0, i32 0, i32 15]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !19}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, !12}
!14 = !{!15, !17}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !18}
!18 = !{i32 3, i32 3}
!19 = !{i32 0, i64 1082130688}
*/
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(77256, 5076);
}

// shader_hash: d8b521f5751ea14b648d3d90c9cf3a6c
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// float_instruction_count: 13
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: d8b521f5751ea14b648d3d90c9cf3a6c.pdb
; shader hash: d8b521f5751ea14b648d3d90c9cf3a6c
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;   output 3 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @draw_ps() {
  %1 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = fmul fast float %1, 2.000000e+00
  %4 = fmul fast float %2, 2.000000e+00
  %5 = fadd fast float %3, -1.000000e+00
  %6 = fadd fast float %4, -1.000000e+00
  %7 = fmul fast float %5, %5
  %8 = fmul fast float %6, %6
  %9 = fadd fast float %8, %7
  %10 = call float @dx.op.unary.f32(i32 24, float %9)  ; Sqrt(value)
  %11 = call float @dx.op.unary.f32(i32 7, float %10)  ; Saturate(value)
  %12 = fsub fast float 1.000000e+00, %11
  %13 = call float @dx.op.unary.f32(i32 23, float %12)  ; Log(value)
  %14 = fmul fast float %13, 3.000000e+00
  %15 = call float @dx.op.unary.f32(i32 21, float %14)  ; Exp(value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!4 = !{void ()* @draw_ps, !"draw_ps", !5, null, !14}
!5 = !{!6, !11, null}
!6 = !{!7, !9}
!7 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!8 = !{i32 0}
!9 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !10}
!10 = !{i32 3, i32 3}
!11 = !{!12}
!12 = !{i32 0, !"SV_Target", i8 9, i8 16, !8, i8 0, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 0, i64 8388864}
*/
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(82332, 3244);
}

// shader_hash: e9f880601bac54b41883afbf7dd54905
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 70
// float_instruction_count: 8
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: e9f880601bac54b41883afbf7dd54905.pdb
; shader hash: e9f880601bac54b41883afbf7dd54905
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint skinning_matrices;                   ; Offset:    8
;           uint color;                               ; Offset:   12
;           uint texture;                             ; Offset:   16
;           float texture_scroll;                     ; Offset:   20
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    24
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, float }

define void @ground_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 24 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %27 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.f32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.f32 %27, 1
  %30 = extractvalue %dx.types.CBufRet.f32 %27, 2
  %31 = extractvalue %dx.types.CBufRet.f32 %27, 3
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 0
  %34 = extractvalue %dx.types.CBufRet.f32 %32, 1
  %35 = extractvalue %dx.types.CBufRet.f32 %32, 2
  %36 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = extractvalue %dx.types.CBufRet.f32 %37, 1
  %40 = extractvalue %dx.types.CBufRet.f32 %37, 2
  %41 = extractvalue %dx.types.CBufRet.f32 %37, 3
  %42 = fmul fast float %23, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %38
  %46 = fmul fast float %24, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %34, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %39
  %50 = fmul fast float %25, %12
  %51 = call float @dx.op.tertiary.f32(i32 46, float %30, float %13, float %50)  ; FMad(a,b,c)
  %52 = call float @dx.op.tertiary.f32(i32 46, float %35, float %14, float %51)  ; FMad(a,b,c)
  %53 = fadd fast float %52, %40
  %54 = fmul fast float %26, %12
  %55 = call float @dx.op.tertiary.f32(i32 46, float %31, float %13, float %54)  ; FMad(a,b,c)
  %56 = call float @dx.op.tertiary.f32(i32 46, float %36, float %14, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %41
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 24, null}
!6 = !{[7 x i32] [i32 5, i32 15, i32 29567, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @ground_vs, !"ground_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::anim_ground_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(85576, 5280);
}

// shader_hash: bb2415c445d29cf08a31585c321c9261
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 52
// float_instruction_count: 25
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: bb2415c445d29cf08a31585c321c9261.pdb
; shader hash: bb2415c445d29cf08a31585c321c9261
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint skinning_matrices;                   ; Offset:    8
;           uint color;                               ; Offset:   12
;           uint texture;                             ; Offset:   16
;           float texture_scroll;                     ; Offset:   20
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    24
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14 }
;   output 1 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14 }
;   output 2 depends on inputs: { 4, 5, 6, 8, 9, 12, 13, 14 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, float }

define void @ground_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 24 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %12 = extractvalue %dx.types.CBufRet.i32 %11, 0
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %15 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 5, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %16 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %15, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %17 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.f32 %17, 1
  %19 = call float @dx.op.dot3.f32(i32 55, float %8, float %9, float %10, float 0xBFE279A740000000, float 0x3FE279A740000000, float 0x3FE279A740000000)  ; Dot3(ax,ay,az,bx,by,bz)
  %20 = call float @dx.op.unary.f32(i32 7, float %19)  ; Saturate(value)
  %21 = fmul fast float %20, 5.000000e-01
  %22 = fadd fast float %21, 5.000000e-01
  %23 = fadd fast float %18, %7
  %24 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %14, %dx.types.Handle %16, float %6, float %23, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %25 = extractvalue %dx.types.ResRet.f32 %24, 0
  %26 = extractvalue %dx.types.ResRet.f32 %24, 1
  %27 = extractvalue %dx.types.ResRet.f32 %24, 2
  %28 = fmul fast float %3, %3
  %29 = fmul fast float %4, %4
  %30 = fadd fast float %29, %28
  %31 = fmul fast float %5, %5
  %32 = fadd fast float %30, %31
  %33 = call float @dx.op.unary.f32(i32 24, float %32)  ; Sqrt(value)
  %34 = fmul fast float %33, 1.250000e-01
  %35 = call float @dx.op.unary.f32(i32 7, float %34)  ; Saturate(value)
  %36 = fmul fast float %25, %22
  %37 = fmul fast float %26, %22
  %38 = fmul fast float %27, %22
  %39 = fsub fast float 7.500000e-01, %36
  %40 = fsub fast float 7.500000e-01, %37
  %41 = fsub fast float 7.500000e-01, %38
  %42 = fmul fast float %39, %35
  %43 = fmul fast float %40, %35
  %44 = fmul fast float %41, %35
  %45 = fadd fast float %42, %36
  %46 = fadd fast float %43, %37
  %47 = fadd fast float %44, %38
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %46)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %47)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 24, null}
!6 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 0, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @ground_ps, !"ground_ps", !8, !3, !22}
!8 = !{!9, !19, null}
!9 = !{!10, !12, !14, !17}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, !16}
!15 = !{i32 1}
!16 = !{i32 3, i32 3}
!17 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !18, i8 2, i32 1, i8 3, i32 3, i8 0, !13}
!18 = !{i32 2}
!19 = !{!20}
!20 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !21}
!21 = !{i32 3, i32 15}
!22 = !{i32 0, i64 3229614336}
*/
auto Shaders::anim_ground_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(90856, 5104);
}

// shader_hash: 0d2bf462b95030f1de951d11eee54d35
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 323
// float_instruction_count: 140
// texture_load_instructions: 22
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: 0d2bf462b95030f1de951d11eee54d35.pdb
; shader hash: 0d2bf462b95030f1de951d11eee54d35
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint skinning_matrices;                   ; Offset:    8
;           uint color;                               ; Offset:   12
;           uint texture;                             ; Offset:   16
;           float texture_scroll;                     ; Offset:   20
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    24
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, float }

define void @anim_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 24 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 80 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=80>
  %11 = extractvalue %dx.types.CBufRet.i32 %4, 2
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 524, i32 64 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=64>
  %14 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %15 = extractvalue %dx.types.ResRet.f32 %14, 0
  %16 = extractvalue %dx.types.ResRet.f32 %14, 1
  %17 = extractvalue %dx.types.ResRet.f32 %14, 2
  %18 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %19 = extractvalue %dx.types.ResRet.f32 %18, 0
  %20 = extractvalue %dx.types.ResRet.f32 %18, 1
  %21 = extractvalue %dx.types.ResRet.f32 %18, 2
  %22 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %23 = extractvalue %dx.types.ResRet.f32 %22, 0
  %24 = extractvalue %dx.types.ResRet.f32 %22, 1
  %25 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %26 = extractvalue %dx.types.ResRet.f32 %25, 0
  %27 = extractvalue %dx.types.ResRet.f32 %25, 1
  %28 = extractvalue %dx.types.ResRet.f32 %25, 2
  %29 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %10, i32 %3, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %30 = extractvalue %dx.types.ResRet.i32 %29, 0
  %31 = extractvalue %dx.types.ResRet.i32 %29, 1
  %32 = extractvalue %dx.types.ResRet.i32 %29, 2
  %33 = extractvalue %dx.types.ResRet.i32 %29, 3
  %34 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 64, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %35 = extractvalue %dx.types.ResRet.f32 %34, 0
  %36 = extractvalue %dx.types.ResRet.f32 %34, 1
  %37 = extractvalue %dx.types.ResRet.f32 %34, 2
  %38 = extractvalue %dx.types.ResRet.f32 %34, 3
  %39 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %30, i32 0, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %40 = extractvalue %dx.types.ResRet.f32 %39, 0
  %41 = extractvalue %dx.types.ResRet.f32 %39, 1
  %42 = extractvalue %dx.types.ResRet.f32 %39, 2
  %43 = extractvalue %dx.types.ResRet.f32 %39, 3
  %44 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %30, i32 16, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %45 = extractvalue %dx.types.ResRet.f32 %44, 0
  %46 = extractvalue %dx.types.ResRet.f32 %44, 1
  %47 = extractvalue %dx.types.ResRet.f32 %44, 2
  %48 = extractvalue %dx.types.ResRet.f32 %44, 3
  %49 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %30, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %50 = extractvalue %dx.types.ResRet.f32 %49, 0
  %51 = extractvalue %dx.types.ResRet.f32 %49, 1
  %52 = extractvalue %dx.types.ResRet.f32 %49, 2
  %53 = extractvalue %dx.types.ResRet.f32 %49, 3
  %54 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %30, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %55 = extractvalue %dx.types.ResRet.f32 %54, 0
  %56 = extractvalue %dx.types.ResRet.f32 %54, 1
  %57 = extractvalue %dx.types.ResRet.f32 %54, 2
  %58 = extractvalue %dx.types.ResRet.f32 %54, 3
  %59 = fmul fast float %40, %35
  %60 = fmul fast float %45, %35
  %61 = fmul fast float %50, %35
  %62 = fmul fast float %55, %35
  %63 = fmul fast float %41, %35
  %64 = fmul fast float %46, %35
  %65 = fmul fast float %51, %35
  %66 = fmul fast float %56, %35
  %67 = fmul fast float %42, %35
  %68 = fmul fast float %47, %35
  %69 = fmul fast float %52, %35
  %70 = fmul fast float %57, %35
  %71 = fmul fast float %43, %35
  %72 = fmul fast float %48, %35
  %73 = fmul fast float %53, %35
  %74 = fmul fast float %58, %35
  %75 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %31, i32 0, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %76 = extractvalue %dx.types.ResRet.f32 %75, 0
  %77 = extractvalue %dx.types.ResRet.f32 %75, 1
  %78 = extractvalue %dx.types.ResRet.f32 %75, 2
  %79 = extractvalue %dx.types.ResRet.f32 %75, 3
  %80 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %31, i32 16, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %81 = extractvalue %dx.types.ResRet.f32 %80, 0
  %82 = extractvalue %dx.types.ResRet.f32 %80, 1
  %83 = extractvalue %dx.types.ResRet.f32 %80, 2
  %84 = extractvalue %dx.types.ResRet.f32 %80, 3
  %85 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %31, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %86 = extractvalue %dx.types.ResRet.f32 %85, 0
  %87 = extractvalue %dx.types.ResRet.f32 %85, 1
  %88 = extractvalue %dx.types.ResRet.f32 %85, 2
  %89 = extractvalue %dx.types.ResRet.f32 %85, 3
  %90 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %31, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %91 = extractvalue %dx.types.ResRet.f32 %90, 0
  %92 = extractvalue %dx.types.ResRet.f32 %90, 1
  %93 = extractvalue %dx.types.ResRet.f32 %90, 2
  %94 = extractvalue %dx.types.ResRet.f32 %90, 3
  %95 = fmul fast float %76, %36
  %96 = fmul fast float %81, %36
  %97 = fmul fast float %86, %36
  %98 = fmul fast float %91, %36
  %99 = fmul fast float %77, %36
  %100 = fmul fast float %82, %36
  %101 = fmul fast float %87, %36
  %102 = fmul fast float %92, %36
  %103 = fmul fast float %78, %36
  %104 = fmul fast float %83, %36
  %105 = fmul fast float %88, %36
  %106 = fmul fast float %93, %36
  %107 = fmul fast float %79, %36
  %108 = fmul fast float %84, %36
  %109 = fmul fast float %89, %36
  %110 = fmul fast float %94, %36
  %111 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %32, i32 0, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %112 = extractvalue %dx.types.ResRet.f32 %111, 0
  %113 = extractvalue %dx.types.ResRet.f32 %111, 1
  %114 = extractvalue %dx.types.ResRet.f32 %111, 2
  %115 = extractvalue %dx.types.ResRet.f32 %111, 3
  %116 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %32, i32 16, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %117 = extractvalue %dx.types.ResRet.f32 %116, 0
  %118 = extractvalue %dx.types.ResRet.f32 %116, 1
  %119 = extractvalue %dx.types.ResRet.f32 %116, 2
  %120 = extractvalue %dx.types.ResRet.f32 %116, 3
  %121 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %32, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %122 = extractvalue %dx.types.ResRet.f32 %121, 0
  %123 = extractvalue %dx.types.ResRet.f32 %121, 1
  %124 = extractvalue %dx.types.ResRet.f32 %121, 2
  %125 = extractvalue %dx.types.ResRet.f32 %121, 3
  %126 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %32, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %127 = extractvalue %dx.types.ResRet.f32 %126, 0
  %128 = extractvalue %dx.types.ResRet.f32 %126, 1
  %129 = extractvalue %dx.types.ResRet.f32 %126, 2
  %130 = extractvalue %dx.types.ResRet.f32 %126, 3
  %131 = fmul fast float %112, %37
  %132 = fmul fast float %117, %37
  %133 = fmul fast float %122, %37
  %134 = fmul fast float %127, %37
  %135 = fmul fast float %113, %37
  %136 = fmul fast float %118, %37
  %137 = fmul fast float %123, %37
  %138 = fmul fast float %128, %37
  %139 = fmul fast float %114, %37
  %140 = fmul fast float %119, %37
  %141 = fmul fast float %124, %37
  %142 = fmul fast float %129, %37
  %143 = fmul fast float %115, %37
  %144 = fmul fast float %120, %37
  %145 = fmul fast float %125, %37
  %146 = fmul fast float %130, %37
  %147 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %33, i32 0, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %148 = extractvalue %dx.types.ResRet.f32 %147, 0
  %149 = extractvalue %dx.types.ResRet.f32 %147, 1
  %150 = extractvalue %dx.types.ResRet.f32 %147, 2
  %151 = extractvalue %dx.types.ResRet.f32 %147, 3
  %152 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %33, i32 16, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %153 = extractvalue %dx.types.ResRet.f32 %152, 0
  %154 = extractvalue %dx.types.ResRet.f32 %152, 1
  %155 = extractvalue %dx.types.ResRet.f32 %152, 2
  %156 = extractvalue %dx.types.ResRet.f32 %152, 3
  %157 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %33, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %158 = extractvalue %dx.types.ResRet.f32 %157, 0
  %159 = extractvalue %dx.types.ResRet.f32 %157, 1
  %160 = extractvalue %dx.types.ResRet.f32 %157, 2
  %161 = extractvalue %dx.types.ResRet.f32 %157, 3
  %162 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %33, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %163 = extractvalue %dx.types.ResRet.f32 %162, 0
  %164 = extractvalue %dx.types.ResRet.f32 %162, 1
  %165 = extractvalue %dx.types.ResRet.f32 %162, 2
  %166 = extractvalue %dx.types.ResRet.f32 %162, 3
  %167 = fmul fast float %148, %38
  %168 = fmul fast float %153, %38
  %169 = fmul fast float %158, %38
  %170 = fmul fast float %163, %38
  %171 = fmul fast float %149, %38
  %172 = fmul fast float %154, %38
  %173 = fmul fast float %159, %38
  %174 = fmul fast float %164, %38
  %175 = fmul fast float %150, %38
  %176 = fmul fast float %155, %38
  %177 = fmul fast float %160, %38
  %178 = fmul fast float %165, %38
  %179 = fmul fast float %151, %38
  %180 = fmul fast float %156, %38
  %181 = fmul fast float %161, %38
  %182 = fmul fast float %166, %38
  %183 = fadd fast float %95, %59
  %184 = fadd fast float %96, %60
  %185 = fadd fast float %97, %61
  %186 = fadd fast float %98, %62
  %187 = fadd fast float %99, %63
  %188 = fadd fast float %100, %64
  %189 = fadd fast float %101, %65
  %190 = fadd fast float %102, %66
  %191 = fadd fast float %103, %67
  %192 = fadd fast float %104, %68
  %193 = fadd fast float %105, %69
  %194 = fadd fast float %106, %70
  %195 = fadd fast float %107, %71
  %196 = fadd fast float %108, %72
  %197 = fadd fast float %109, %73
  %198 = fadd fast float %110, %74
  %199 = fadd fast float %183, %131
  %200 = fadd fast float %184, %132
  %201 = fadd fast float %185, %133
  %202 = fadd fast float %186, %134
  %203 = fadd fast float %187, %135
  %204 = fadd fast float %188, %136
  %205 = fadd fast float %189, %137
  %206 = fadd fast float %190, %138
  %207 = fadd fast float %191, %139
  %208 = fadd fast float %192, %140
  %209 = fadd fast float %193, %141
  %210 = fadd fast float %194, %142
  %211 = fadd fast float %195, %143
  %212 = fadd fast float %196, %144
  %213 = fadd fast float %197, %145
  %214 = fadd fast float %198, %146
  %215 = fadd fast float %199, %167
  %216 = fadd fast float %200, %168
  %217 = fadd fast float %201, %169
  %218 = fadd fast float %202, %170
  %219 = fadd fast float %203, %171
  %220 = fadd fast float %204, %172
  %221 = fadd fast float %205, %173
  %222 = fadd fast float %206, %174
  %223 = fadd fast float %207, %175
  %224 = fadd fast float %208, %176
  %225 = fadd fast float %209, %177
  %226 = fadd fast float %210, %178
  %227 = fadd fast float %211, %179
  %228 = fadd fast float %212, %180
  %229 = fadd fast float %213, %181
  %230 = fadd fast float %214, %182
  %231 = fmul fast float %215, %15
  %232 = call float @dx.op.tertiary.f32(i32 46, float %216, float %16, float %231)  ; FMad(a,b,c)
  %233 = call float @dx.op.tertiary.f32(i32 46, float %217, float %17, float %232)  ; FMad(a,b,c)
  %234 = fadd fast float %218, %233
  %235 = fmul fast float %219, %15
  %236 = call float @dx.op.tertiary.f32(i32 46, float %220, float %16, float %235)  ; FMad(a,b,c)
  %237 = call float @dx.op.tertiary.f32(i32 46, float %221, float %17, float %236)  ; FMad(a,b,c)
  %238 = fadd fast float %222, %237
  %239 = fmul fast float %223, %15
  %240 = call float @dx.op.tertiary.f32(i32 46, float %224, float %16, float %239)  ; FMad(a,b,c)
  %241 = call float @dx.op.tertiary.f32(i32 46, float %225, float %17, float %240)  ; FMad(a,b,c)
  %242 = fadd fast float %226, %241
  %243 = fmul fast float %227, %15
  %244 = call float @dx.op.tertiary.f32(i32 46, float %228, float %16, float %243)  ; FMad(a,b,c)
  %245 = call float @dx.op.tertiary.f32(i32 46, float %229, float %17, float %244)  ; FMad(a,b,c)
  %246 = fadd fast float %230, %245
  %247 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %248 = extractvalue %dx.types.CBufRet.f32 %247, 0
  %249 = extractvalue %dx.types.CBufRet.f32 %247, 1
  %250 = extractvalue %dx.types.CBufRet.f32 %247, 2
  %251 = extractvalue %dx.types.CBufRet.f32 %247, 3
  %252 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %253 = extractvalue %dx.types.CBufRet.f32 %252, 0
  %254 = extractvalue %dx.types.CBufRet.f32 %252, 1
  %255 = extractvalue %dx.types.CBufRet.f32 %252, 2
  %256 = extractvalue %dx.types.CBufRet.f32 %252, 3
  %257 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %258 = extractvalue %dx.types.CBufRet.f32 %257, 0
  %259 = extractvalue %dx.types.CBufRet.f32 %257, 1
  %260 = extractvalue %dx.types.CBufRet.f32 %257, 2
  %261 = extractvalue %dx.types.CBufRet.f32 %257, 3
  %262 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %263 = extractvalue %dx.types.CBufRet.f32 %262, 0
  %264 = extractvalue %dx.types.CBufRet.f32 %262, 1
  %265 = extractvalue %dx.types.CBufRet.f32 %262, 2
  %266 = extractvalue %dx.types.CBufRet.f32 %262, 3
  %267 = fmul fast float %248, %234
  %268 = call float @dx.op.tertiary.f32(i32 46, float %253, float %238, float %267)  ; FMad(a,b,c)
  %269 = call float @dx.op.tertiary.f32(i32 46, float %258, float %242, float %268)  ; FMad(a,b,c)
  %270 = call float @dx.op.tertiary.f32(i32 46, float %263, float %246, float %269)  ; FMad(a,b,c)
  %271 = fmul fast float %249, %234
  %272 = call float @dx.op.tertiary.f32(i32 46, float %254, float %238, float %271)  ; FMad(a,b,c)
  %273 = call float @dx.op.tertiary.f32(i32 46, float %259, float %242, float %272)  ; FMad(a,b,c)
  %274 = call float @dx.op.tertiary.f32(i32 46, float %264, float %246, float %273)  ; FMad(a,b,c)
  %275 = fmul fast float %250, %234
  %276 = call float @dx.op.tertiary.f32(i32 46, float %255, float %238, float %275)  ; FMad(a,b,c)
  %277 = call float @dx.op.tertiary.f32(i32 46, float %260, float %242, float %276)  ; FMad(a,b,c)
  %278 = call float @dx.op.tertiary.f32(i32 46, float %265, float %246, float %277)  ; FMad(a,b,c)
  %279 = fmul fast float %251, %234
  %280 = call float @dx.op.tertiary.f32(i32 46, float %256, float %238, float %279)  ; FMad(a,b,c)
  %281 = call float @dx.op.tertiary.f32(i32 46, float %261, float %242, float %280)  ; FMad(a,b,c)
  %282 = call float @dx.op.tertiary.f32(i32 46, float %266, float %246, float %281)  ; FMad(a,b,c)
  %283 = fmul fast float %215, %19
  %284 = call float @dx.op.tertiary.f32(i32 46, float %216, float %20, float %283)  ; FMad(a,b,c)
  %285 = call float @dx.op.tertiary.f32(i32 46, float %217, float %21, float %284)  ; FMad(a,b,c)
  %286 = fmul fast float %219, %19
  %287 = call float @dx.op.tertiary.f32(i32 46, float %220, float %20, float %286)  ; FMad(a,b,c)
  %288 = call float @dx.op.tertiary.f32(i32 46, float %221, float %21, float %287)  ; FMad(a,b,c)
  %289 = fmul fast float %223, %19
  %290 = call float @dx.op.tertiary.f32(i32 46, float %224, float %20, float %289)  ; FMad(a,b,c)
  %291 = call float @dx.op.tertiary.f32(i32 46, float %225, float %21, float %290)  ; FMad(a,b,c)
  %292 = call float @dx.op.dot3.f32(i32 55, float %285, float %288, float %291, float %285, float %288, float %291)  ; Dot3(ax,ay,az,bx,by,bz)
  %293 = call float @dx.op.unary.f32(i32 25, float %292)  ; Rsqrt(value)
  %294 = fmul fast float %293, %285
  %295 = fmul fast float %293, %288
  %296 = fmul fast float %293, %291
  %297 = fmul fast float %215, %26
  %298 = call float @dx.op.tertiary.f32(i32 46, float %216, float %27, float %297)  ; FMad(a,b,c)
  %299 = call float @dx.op.tertiary.f32(i32 46, float %217, float %28, float %298)  ; FMad(a,b,c)
  %300 = fmul fast float %219, %26
  %301 = call float @dx.op.tertiary.f32(i32 46, float %220, float %27, float %300)  ; FMad(a,b,c)
  %302 = call float @dx.op.tertiary.f32(i32 46, float %221, float %28, float %301)  ; FMad(a,b,c)
  %303 = fmul fast float %223, %26
  %304 = call float @dx.op.tertiary.f32(i32 46, float %224, float %27, float %303)  ; FMad(a,b,c)
  %305 = call float @dx.op.tertiary.f32(i32 46, float %225, float %28, float %304)  ; FMad(a,b,c)
  %306 = call float @dx.op.dot3.f32(i32 55, float %299, float %302, float %305, float %299, float %302, float %305)  ; Dot3(ax,ay,az,bx,by,bz)
  %307 = call float @dx.op.unary.f32(i32 25, float %306)  ; Rsqrt(value)
  %308 = fmul fast float %307, %299
  %309 = fmul fast float %307, %302
  %310 = fmul fast float %307, %305
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %270)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %274)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %278)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %282)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %294)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %295)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %296)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %23)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %24)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %308)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %309)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %310)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 24, null}
!6 = !{[7 x i32] [i32 5, i32 15, i32 29567, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @anim_vs, !"anim_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::anim_anim_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(95960, 7092);
}

// shader_hash: 84e53889c321f6c1f76962650f96da99
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 29
// float_instruction_count: 13
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 84e53889c321f6c1f76962650f96da99.pdb
; shader hash: 84e53889c321f6c1f76962650f96da99
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint skinning_matrices;                   ; Offset:    8
;           uint color;                               ; Offset:   12
;           uint texture;                             ; Offset:   16
;           float texture_scroll;                     ; Offset:   20
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    24
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6 }
;   output 1 depends on inputs: { 4, 5, 6 }
;   output 2 depends on inputs: { 4, 5, 6 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.fouri32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, float }

define void @anim_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 24 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %8 = call %dx.types.fouri32 @dx.op.unpack4x8.i32(i32 219, i8 0, i32 %7)  ; Unpack4x8(unpackMode,pk)
  %9 = extractvalue %dx.types.fouri32 %8, 0
  %10 = extractvalue %dx.types.fouri32 %8, 1
  %11 = extractvalue %dx.types.fouri32 %8, 2
  %12 = uitofp i32 %9 to float
  %13 = uitofp i32 %10 to float
  %14 = uitofp i32 %11 to float
  %15 = fmul fast float %12, 0x3F70101020000000
  %16 = fmul fast float %13, 0x3F70101020000000
  %17 = fmul fast float %14, 0x3F70101020000000
  %18 = call float @dx.op.dot3.f32(i32 55, float %3, float %4, float %5, float 0xBFE279A740000000, float 0x3FE279A740000000, float 0x3FE279A740000000)  ; Dot3(ax,ay,az,bx,by,bz)
  %19 = call float @dx.op.unary.f32(i32 7, float %18)  ; Saturate(value)
  %20 = fmul fast float %19, 5.000000e-01
  %21 = fadd fast float %20, 5.000000e-01
  %22 = fmul fast float %15, %21
  %23 = fmul fast float %16, %21
  %24 = fmul fast float %17, %21
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %22)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %23)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %24)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.fouri32 @dx.op.unpack4x8.i32(i32, i8, i32) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 24, null}
!6 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @anim_ps, !"anim_ps", !8, !3, !21}
!8 = !{!9, !18, null}
!9 = !{!10, !12, !14, !16}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!15 = !{i32 1}
!16 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !17, i8 2, i32 1, i8 3, i32 3, i8 0, null}
!17 = !{i32 2}
!18 = !{!19}
!19 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !20}
!20 = !{i32 3, i32 15}
!21 = !{i32 0, i64 8388864}
*/
auto Shaders::anim_anim_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(103052, 4624);
}

// shader_hash: 4c24160a228de20f6e6b67a3c4dc1f64
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 32
// float_instruction_count: 4
// texture_load_instructions: 3
// texture_store_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: 4c24160a228de20f6e6b67a3c4dc1f64.pdb
; shader hash: 4c24160a228de20f6e6b67a3c4dc1f64
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(32,1,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @sim_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 4620, i32 28 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=28>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 12)  ; CBufferLoadLegacy(handle,regIndex)
  %22 = extractvalue %dx.types.CBufRet.f32 %21, 3
  %23 = fmul fast float %22, %20
  %24 = extractvalue %dx.types.CBufRet.f32 %21, 2
  %25 = fmul fast float %23, %24
  %26 = fadd fast float %25, %12
  %27 = fcmp fast ogt float %26, 2.000000e+00
  %28 = select i1 %27, float -2.000000e+00, float %26
  call void @dx.op.rawBufferStore.f32(i32 140, %dx.types.Handle %10, i32 %3, i32 0, float %28, float %13, float %14, float undef, i8 7, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  call void @dx.op.rawBufferStore.f32(i32 140, %dx.types.Handle %10, i32 %3, i32 12, float %16, float %17, float %18, float undef, i8 7, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  call void @dx.op.rawBufferStore.f32(i32 140, %dx.types.Handle %10, i32 %3, i32 24, float %20, float undef, float undef, float undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #1

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.f32(i32, %dx.types.Handle, i32, i32, float, float, float, float, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{void ()* @sim_cs, !"sim_cs", null, !3, !7}
!7 = !{i32 0, i64 9672065280, i32 4, !8}
!8 = !{i32 32, i32 1, i32 1}
*/
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(107676, 4012);
}

// shader_hash: 538af1d555e6229ed12eb7826b7744f7
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 8
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: 538af1d555e6229ed12eb7826b7744f7.pdb
; shader hash: 538af1d555e6229ed12eb7826b7744f7
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(1,1,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @reset_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %4 = extractvalue %dx.types.CBufRet.i32 %3, 3
  %5 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %4, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %6 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %5, %dx.types.ResourceProperties { i32 4108, i32 4 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=4>
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %6, i32 0, i32 0, i32 0, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  ret void
}

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i8, i32) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{void ()* @reset_cs, !"reset_cs", null, !3, !7}
!7 = !{i32 0, i64 9672065280, i32 4, !8}
!8 = !{i32 1, i32 1, i32 1}
*/
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(111688, 3436);
}

// shader_hash: cda447ac08bc8e96d95137c291d7ad57
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 322
// temp_array_count: 192
// dynamic_flow_control_count: 9
// array_instruction_count: 22
// float_instruction_count: 124
// int_instruction_count: 19
// uint_instruction_count: 5
// texture_load_instructions: 1
// barrier_instructions: 3
// interlocked_instructions: 2
// texture_store_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: cda447ac08bc8e96d95137c291d7ad57.pdb
; shader hash: cda447ac08bc8e96d95137c291d7ad57
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(16,16,1)
;
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

@"\01?gs_light_indices@@3PAIA" = external addrspace(3) global [32 x i32], align 4
@"\01?gs_light_count@@3IA" = external addrspace(3) global i32, align 4
@"\01?gs_light_offset@@3IA" = external addrspace(3) global i32, align 4
@"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim" = addrspace(3) global [16 x float] undef, align 4

define void @cull_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.groupId.i32(i32 94, i32 0)  ; GroupId(component)
  %4 = call i32 @dx.op.groupId.i32(i32 94, i32 1)  ; GroupId(component)
  %5 = call i32 @dx.op.flattenedThreadIdInGroup.i32(i32 96)  ; FlattenedThreadIdInGroup()
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 1
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 524, i32 28 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=28>
  %13 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %14 = extractvalue %dx.types.CBufRet.i32 %13, 0
  %15 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %14, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %16 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %15, %dx.types.ResourceProperties { i32 4098, i32 261 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<U32>
  %17 = extractvalue %dx.types.CBufRet.i32 %13, 1
  %18 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %17, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %19 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %18, %dx.types.ResourceProperties { i32 4098, i32 261 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<U32>
  %20 = extractvalue %dx.types.CBufRet.i32 %13, 2
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %20, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 4108, i32 4 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=4>
  %23 = extractvalue %dx.types.CBufRet.i32 %13, 3
  %24 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %23, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %25 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %24, %dx.types.ResourceProperties { i32 4108, i32 4 })  ; AnnotateHandle(res,props)  resource: RWStructuredBuffer<stride=4>
  %26 = icmp eq i32 %5, 0
  br i1 %26, label %27, label %199

; <label>:27                                      ; preds = %0
  %28 = shl i32 %3, 4
  %29 = uitofp i32 %28 to float
  %30 = shl i32 %4, 4
  %31 = uitofp i32 %30 to float
  %32 = fadd fast float %29, 1.600000e+01
  %33 = fadd fast float %31, 1.600000e+01
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 12)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = fmul fast float %29, 2.000000e+00
  %38 = fdiv fast float %37, %35
  %39 = fadd fast float %38, -1.000000e+00
  %40 = fmul fast float %31, 2.000000e+00
  %41 = fdiv fast float %40, %36
  %42 = fadd fast float %41, -1.000000e+00
  %43 = fsub fast float -0.000000e+00, %42
  %44 = fmul fast float %32, 2.000000e+00
  %45 = fdiv fast float %44, %35
  %46 = fadd fast float %45, -1.000000e+00
  %47 = fmul fast float %33, 2.000000e+00
  %48 = fdiv fast float %47, %36
  %49 = fadd fast float %48, -1.000000e+00
  %50 = fsub fast float -0.000000e+00, %49
  %51 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %52 = extractvalue %dx.types.CBufRet.f32 %51, 0
  %53 = extractvalue %dx.types.CBufRet.f32 %51, 1
  %54 = extractvalue %dx.types.CBufRet.f32 %51, 2
  %55 = extractvalue %dx.types.CBufRet.f32 %51, 3
  %56 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %57 = extractvalue %dx.types.CBufRet.f32 %56, 0
  %58 = extractvalue %dx.types.CBufRet.f32 %56, 1
  %59 = extractvalue %dx.types.CBufRet.f32 %56, 2
  %60 = extractvalue %dx.types.CBufRet.f32 %56, 3
  %61 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %62 = extractvalue %dx.types.CBufRet.f32 %61, 0
  %63 = extractvalue %dx.types.CBufRet.f32 %61, 1
  %64 = extractvalue %dx.types.CBufRet.f32 %61, 2
  %65 = extractvalue %dx.types.CBufRet.f32 %61, 3
  %66 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 7)  ; CBufferLoadLegacy(handle,regIndex)
  %67 = extractvalue %dx.types.CBufRet.f32 %66, 0
  %68 = extractvalue %dx.types.CBufRet.f32 %66, 1
  %69 = extractvalue %dx.types.CBufRet.f32 %66, 2
  %70 = extractvalue %dx.types.CBufRet.f32 %66, 3
  %71 = fmul fast float %52, %39
  %72 = call float @dx.op.tertiary.f32(i32 46, float %57, float %43, float %71)  ; FMad(a,b,c)
  %73 = call float @dx.op.tertiary.f32(i32 46, float %62, float -1.000000e+00, float %72)  ; FMad(a,b,c)
  %74 = fadd fast float %73, %67
  %75 = fmul fast float %53, %39
  %76 = call float @dx.op.tertiary.f32(i32 46, float %58, float %43, float %75)  ; FMad(a,b,c)
  %77 = call float @dx.op.tertiary.f32(i32 46, float %63, float -1.000000e+00, float %76)  ; FMad(a,b,c)
  %78 = fadd fast float %77, %68
  %79 = fmul fast float %54, %39
  %80 = call float @dx.op.tertiary.f32(i32 46, float %59, float %43, float %79)  ; FMad(a,b,c)
  %81 = call float @dx.op.tertiary.f32(i32 46, float %64, float -1.000000e+00, float %80)  ; FMad(a,b,c)
  %82 = fadd fast float %81, %69
  %83 = fmul fast float %55, %39
  %84 = call float @dx.op.tertiary.f32(i32 46, float %60, float %43, float %83)  ; FMad(a,b,c)
  %85 = call float @dx.op.tertiary.f32(i32 46, float %65, float -1.000000e+00, float %84)  ; FMad(a,b,c)
  %86 = fadd fast float %85, %70
  %87 = fdiv fast float %74, %86
  %88 = fdiv fast float %78, %86
  %89 = fdiv fast float %82, %86
  %90 = fmul fast float %52, %46
  %91 = call float @dx.op.tertiary.f32(i32 46, float %57, float %43, float %90)  ; FMad(a,b,c)
  %92 = call float @dx.op.tertiary.f32(i32 46, float %62, float -1.000000e+00, float %91)  ; FMad(a,b,c)
  %93 = fadd fast float %92, %67
  %94 = fmul fast float %53, %46
  %95 = call float @dx.op.tertiary.f32(i32 46, float %58, float %43, float %94)  ; FMad(a,b,c)
  %96 = call float @dx.op.tertiary.f32(i32 46, float %63, float -1.000000e+00, float %95)  ; FMad(a,b,c)
  %97 = fadd fast float %96, %68
  %98 = fmul fast float %54, %46
  %99 = call float @dx.op.tertiary.f32(i32 46, float %59, float %43, float %98)  ; FMad(a,b,c)
  %100 = call float @dx.op.tertiary.f32(i32 46, float %64, float -1.000000e+00, float %99)  ; FMad(a,b,c)
  %101 = fadd fast float %100, %69
  %102 = fmul fast float %55, %46
  %103 = call float @dx.op.tertiary.f32(i32 46, float %60, float %43, float %102)  ; FMad(a,b,c)
  %104 = call float @dx.op.tertiary.f32(i32 46, float %65, float -1.000000e+00, float %103)  ; FMad(a,b,c)
  %105 = fadd fast float %104, %70
  %106 = fdiv fast float %93, %105
  %107 = fdiv fast float %97, %105
  %108 = fdiv fast float %101, %105
  %109 = call float @dx.op.tertiary.f32(i32 46, float %57, float %50, float %71)  ; FMad(a,b,c)
  %110 = call float @dx.op.tertiary.f32(i32 46, float %62, float -1.000000e+00, float %109)  ; FMad(a,b,c)
  %111 = fadd fast float %110, %67
  %112 = call float @dx.op.tertiary.f32(i32 46, float %58, float %50, float %75)  ; FMad(a,b,c)
  %113 = call float @dx.op.tertiary.f32(i32 46, float %63, float -1.000000e+00, float %112)  ; FMad(a,b,c)
  %114 = fadd fast float %113, %68
  %115 = call float @dx.op.tertiary.f32(i32 46, float %59, float %50, float %79)  ; FMad(a,b,c)
  %116 = call float @dx.op.tertiary.f32(i32 46, float %64, float -1.000000e+00, float %115)  ; FMad(a,b,c)
  %117 = fadd fast float %116, %69
  %118 = call float @dx.op.tertiary.f32(i32 46, float %60, float %50, float %83)  ; FMad(a,b,c)
  %119 = call float @dx.op.tertiary.f32(i32 46, float %65, float -1.000000e+00, float %118)  ; FMad(a,b,c)
  %120 = fadd fast float %119, %70
  %121 = fdiv fast float %111, %120
  %122 = fdiv fast float %114, %120
  %123 = fdiv fast float %117, %120
  %124 = call float @dx.op.tertiary.f32(i32 46, float %57, float %50, float %90)  ; FMad(a,b,c)
  %125 = call float @dx.op.tertiary.f32(i32 46, float %62, float -1.000000e+00, float %124)  ; FMad(a,b,c)
  %126 = fadd fast float %125, %67
  %127 = call float @dx.op.tertiary.f32(i32 46, float %58, float %50, float %94)  ; FMad(a,b,c)
  %128 = call float @dx.op.tertiary.f32(i32 46, float %63, float -1.000000e+00, float %127)  ; FMad(a,b,c)
  %129 = fadd fast float %128, %68
  %130 = call float @dx.op.tertiary.f32(i32 46, float %59, float %50, float %98)  ; FMad(a,b,c)
  %131 = call float @dx.op.tertiary.f32(i32 46, float %64, float -1.000000e+00, float %130)  ; FMad(a,b,c)
  %132 = fadd fast float %131, %69
  %133 = call float @dx.op.tertiary.f32(i32 46, float %60, float %50, float %102)  ; FMad(a,b,c)
  %134 = call float @dx.op.tertiary.f32(i32 46, float %65, float -1.000000e+00, float %133)  ; FMad(a,b,c)
  %135 = fadd fast float %134, %70
  %136 = fdiv fast float %126, %135
  %137 = fdiv fast float %129, %135
  %138 = fdiv fast float %132, %135
  %139 = fmul fast float %122, %89
  %140 = fmul fast float %123, %88
  %141 = fsub fast float %139, %140
  %142 = fmul fast float %123, %87
  %143 = fmul fast float %121, %89
  %144 = fsub fast float %142, %143
  %145 = fmul fast float %121, %88
  %146 = fmul fast float %122, %87
  %147 = fsub fast float %145, %146
  %148 = call float @dx.op.dot3.f32(i32 55, float %141, float %144, float %147, float %141, float %144, float %147)  ; Dot3(ax,ay,az,bx,by,bz)
  %149 = call float @dx.op.unary.f32(i32 25, float %148)  ; Rsqrt(value)
  %150 = fmul fast float %149, %141
  %151 = fmul fast float %149, %144
  %152 = fmul fast float %149, %147
  %153 = call float @dx.op.dot3.f32(i32 55, float %150, float %151, float %152, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  store float %150, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 0), align 4
  store float %151, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 1), align 4
  store float %152, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 2), align 4
  store float %153, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 3), align 4
  %154 = fmul fast float %138, %107
  %155 = fmul fast float %137, %108
  %156 = fsub fast float %154, %155
  %157 = fmul fast float %136, %108
  %158 = fmul fast float %138, %106
  %159 = fsub fast float %157, %158
  %160 = fmul fast float %137, %106
  %161 = fmul fast float %136, %107
  %162 = fsub fast float %160, %161
  %163 = call float @dx.op.dot3.f32(i32 55, float %156, float %159, float %162, float %156, float %159, float %162)  ; Dot3(ax,ay,az,bx,by,bz)
  %164 = call float @dx.op.unary.f32(i32 25, float %163)  ; Rsqrt(value)
  %165 = fmul fast float %164, %156
  %166 = fmul fast float %164, %159
  %167 = fmul fast float %164, %162
  %168 = call float @dx.op.dot3.f32(i32 55, float %165, float %166, float %167, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  store float %165, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 4), align 4
  store float %166, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 5), align 4
  store float %167, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 6), align 4
  store float %168, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 7), align 4
  %169 = fmul fast float %108, %88
  %170 = fmul fast float %107, %89
  %171 = fsub fast float %169, %170
  %172 = fmul fast float %106, %89
  %173 = fmul fast float %108, %87
  %174 = fsub fast float %172, %173
  %175 = fmul fast float %107, %87
  %176 = fmul fast float %106, %88
  %177 = fsub fast float %175, %176
  %178 = call float @dx.op.dot3.f32(i32 55, float %171, float %174, float %177, float %171, float %174, float %177)  ; Dot3(ax,ay,az,bx,by,bz)
  %179 = call float @dx.op.unary.f32(i32 25, float %178)  ; Rsqrt(value)
  %180 = fmul fast float %179, %171
  %181 = fmul fast float %179, %174
  %182 = fmul fast float %179, %177
  %183 = call float @dx.op.dot3.f32(i32 55, float %180, float %181, float %182, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  store float %180, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 8), align 4
  store float %181, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 9), align 4
  store float %182, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 10), align 4
  store float %183, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 11), align 4
  %184 = fmul fast float %137, %123
  %185 = fmul fast float %138, %122
  %186 = fsub fast float %184, %185
  %187 = fmul fast float %138, %121
  %188 = fmul fast float %136, %123
  %189 = fsub fast float %187, %188
  %190 = fmul fast float %136, %122
  %191 = fmul fast float %137, %121
  %192 = fsub fast float %190, %191
  %193 = call float @dx.op.dot3.f32(i32 55, float %186, float %189, float %192, float %186, float %189, float %192)  ; Dot3(ax,ay,az,bx,by,bz)
  %194 = call float @dx.op.unary.f32(i32 25, float %193)  ; Rsqrt(value)
  %195 = fmul fast float %194, %186
  %196 = fmul fast float %194, %189
  %197 = fmul fast float %194, %192
  %198 = call float @dx.op.dot3.f32(i32 55, float %195, float %196, float %197, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00)  ; Dot3(ax,ay,az,bx,by,bz)
  store float %195, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 12), align 4
  store float %196, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 13), align 4
  store float %197, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 14), align 4
  store float %198, float addrspace(3)* getelementptr inbounds ([16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 15), align 4
  store i32 0, i32 addrspace(3)* @"\01?gs_light_count@@3IA", align 4, !tbaa !9
  store i32 0, i32 addrspace(3)* @"\01?gs_light_offset@@3IA", align 4, !tbaa !9
  br label %199

; <label>:199                                     ; preds = %27, %0
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %200 = icmp ult i32 %5, 32
  br i1 %200, label %201, label %271

; <label>:201                                     ; preds = %199
  %202 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %12, i32 %5, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %203 = extractvalue %dx.types.ResRet.f32 %202, 0
  %204 = extractvalue %dx.types.ResRet.f32 %202, 1
  %205 = extractvalue %dx.types.ResRet.f32 %202, 2
  %206 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 13)  ; CBufferLoadLegacy(handle,regIndex)
  %207 = extractvalue %dx.types.CBufRet.f32 %206, 0
  %208 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 8)  ; CBufferLoadLegacy(handle,regIndex)
  %209 = extractvalue %dx.types.CBufRet.f32 %208, 0
  %210 = extractvalue %dx.types.CBufRet.f32 %208, 1
  %211 = extractvalue %dx.types.CBufRet.f32 %208, 2
  %212 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 9)  ; CBufferLoadLegacy(handle,regIndex)
  %213 = extractvalue %dx.types.CBufRet.f32 %212, 0
  %214 = extractvalue %dx.types.CBufRet.f32 %212, 1
  %215 = extractvalue %dx.types.CBufRet.f32 %212, 2
  %216 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 10)  ; CBufferLoadLegacy(handle,regIndex)
  %217 = extractvalue %dx.types.CBufRet.f32 %216, 0
  %218 = extractvalue %dx.types.CBufRet.f32 %216, 1
  %219 = extractvalue %dx.types.CBufRet.f32 %216, 2
  %220 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 11)  ; CBufferLoadLegacy(handle,regIndex)
  %221 = extractvalue %dx.types.CBufRet.f32 %220, 0
  %222 = extractvalue %dx.types.CBufRet.f32 %220, 1
  %223 = extractvalue %dx.types.CBufRet.f32 %220, 2
  br label %226

; <label>:224                                     ; preds = %226
  %225 = icmp eq i32 %263, 0
  br i1 %225, label %266, label %271

; <label>:226                                     ; preds = %226, %201
  %227 = phi i32 [ 0, %201 ], [ %263, %226 ]
  %228 = phi i32 [ 0, %201 ], [ %264, %226 ]
  %229 = mul i32 %228, 4
  %230 = add i32 0, %229
  %231 = getelementptr [16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 %230
  %232 = load float, float addrspace(3)* %231, align 4
  %233 = mul i32 %228, 4
  %234 = add i32 1, %233
  %235 = getelementptr [16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 %234
  %236 = load float, float addrspace(3)* %235, align 4
  %237 = mul i32 %228, 4
  %238 = add i32 2, %237
  %239 = getelementptr [16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 %238
  %240 = load float, float addrspace(3)* %239, align 4
  %241 = mul i32 %228, 4
  %242 = add i32 3, %241
  %243 = getelementptr [16 x float], [16 x float] addrspace(3)* @"\01?gs_frustum_planes@@3PAV?$vector@M$03@@A.v.1dim", i32 0, i32 %242
  %244 = load float, float addrspace(3)* %243, align 4
  %245 = fmul fast float %209, %203
  %246 = call float @dx.op.tertiary.f32(i32 46, float %213, float %204, float %245)  ; FMad(a,b,c)
  %247 = call float @dx.op.tertiary.f32(i32 46, float %217, float %205, float %246)  ; FMad(a,b,c)
  %248 = fadd fast float %247, %221
  %249 = fmul fast float %210, %203
  %250 = call float @dx.op.tertiary.f32(i32 46, float %214, float %204, float %249)  ; FMad(a,b,c)
  %251 = call float @dx.op.tertiary.f32(i32 46, float %218, float %205, float %250)  ; FMad(a,b,c)
  %252 = fadd fast float %251, %222
  %253 = fmul fast float %211, %203
  %254 = call float @dx.op.tertiary.f32(i32 46, float %215, float %204, float %253)  ; FMad(a,b,c)
  %255 = call float @dx.op.tertiary.f32(i32 46, float %219, float %205, float %254)  ; FMad(a,b,c)
  %256 = fadd fast float %255, %223
  %257 = call float @dx.op.dot3.f32(i32 55, float %232, float %236, float %240, float %248, float %252, float %256)  ; Dot3(ax,ay,az,bx,by,bz)
  %258 = fsub fast float %257, %244
  %259 = fsub fast float -0.000000e+00, %207
  %260 = fcmp fast olt float %258, %259
  %261 = zext i1 %260 to i32
  %262 = shl i32 %261, %228
  %263 = or i32 %262, %227
  %264 = add nuw nsw i32 %228, 1
  %265 = icmp eq i32 %264, 4
  br i1 %265, label %224, label %226

; <label>:266                                     ; preds = %224
  %267 = atomicrmw add i32 addrspace(3)* @"\01?gs_light_count@@3IA", i32 1 seq_cst
  %268 = icmp ult i32 %267, 8
  br i1 %268, label %269, label %271

; <label>:269                                     ; preds = %266
  %270 = getelementptr [32 x i32], [32 x i32] addrspace(3)* @"\01?gs_light_indices@@3PAIA", i32 0, i32 %267
  store i32 %5, i32 addrspace(3)* %270, align 4, !tbaa !9
  br label %271

; <label>:271                                     ; preds = %269, %266, %224, %199
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  br i1 %26, label %272, label %281

; <label>:272                                     ; preds = %271
  %273 = load i32, i32 addrspace(3)* @"\01?gs_light_count@@3IA", align 4, !tbaa !9
  %274 = icmp eq i32 %273, 0
  br i1 %274, label %278, label %275

; <label>:275                                     ; preds = %272
  %276 = call i32 @dx.op.atomicBinOp.i32(i32 78, %dx.types.Handle %25, i32 0, i32 0, i32 0, i32 undef, i32 %273)  ; AtomicBinOp(handle,atomicOp,offset0,offset1,offset2,newValue)
  %277 = load i32, i32 addrspace(3)* @"\01?gs_light_count@@3IA", align 4, !tbaa !9
  br label %278

; <label>:278                                     ; preds = %275, %272
  %279 = phi i32 [ 0, %272 ], [ %277, %275 ]
  %280 = phi i32 [ 0, %272 ], [ %276, %275 ]
  call void @dx.op.textureStore.i32(i32 67, %dx.types.Handle %16, i32 %3, i32 %4, i32 undef, i32 %279, i32 %279, i32 %279, i32 %279, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  call void @dx.op.textureStore.i32(i32 67, %dx.types.Handle %19, i32 %3, i32 %4, i32 undef, i32 %280, i32 %280, i32 %280, i32 %280, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  store i32 %280, i32 addrspace(3)* @"\01?gs_light_offset@@3IA", align 4, !tbaa !9
  br label %281

; <label>:281                                     ; preds = %278, %271
  call void @dx.op.barrier(i32 80, i32 9)  ; Barrier(barrierMode)
  %282 = load i32, i32 addrspace(3)* @"\01?gs_light_offset@@3IA", align 4, !tbaa !9
  %283 = load i32, i32 addrspace(3)* @"\01?gs_light_count@@3IA", align 4, !tbaa !9
  %284 = icmp ult i32 %5, %283
  br i1 %284, label %285, label %287

; <label>:285                                     ; preds = %281
  br label %288

; <label>:286                                     ; preds = %288
  br label %287

; <label>:287                                     ; preds = %286, %281
  ret void

; <label>:288                                     ; preds = %288, %285
  %289 = phi i32 [ %293, %288 ], [ %5, %285 ]
  %290 = getelementptr [32 x i32], [32 x i32] addrspace(3)* @"\01?gs_light_indices@@3PAIA", i32 0, i32 %289
  %291 = load i32, i32 addrspace(3)* %290, align 4, !tbaa !9
  %292 = add i32 %289, %282
  call void @dx.op.rawBufferStore.i32(i32 140, %dx.types.Handle %22, i32 %292, i32 0, i32 %291, i32 undef, i32 undef, i32 undef, i8 1, i32 4)  ; RawBufferStore(uav,index,elementOffset,value0,value1,value2,value3,mask,alignment)
  %293 = add i32 %289, 64
  %294 = load i32, i32 addrspace(3)* @"\01?gs_light_count@@3IA", align 4, !tbaa !9
  %295 = icmp ult i32 %293, %294
  br i1 %295, label %288, label %286
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.groupId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare i32 @dx.op.flattenedThreadIdInGroup.i32(i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #1

; Function Attrs: noduplicate nounwind
declare void @dx.op.barrier(i32, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #1

; Function Attrs: nounwind
declare i32 @dx.op.atomicBinOp.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32) #3

; Function Attrs: nounwind
declare void @dx.op.rawBufferStore.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i8, i32) #3

; Function Attrs: nounwind
declare void @dx.op.textureStore.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32, i8) #3

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { noduplicate nounwind }
attributes #3 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{void ()* @cull_cs, !"cull_cs", null, !3, !7}
!7 = !{i32 0, i64 9672065280, i32 4, !8}
!8 = !{i32 16, i32 16, i32 1}
!9 = !{!10, !10, i64 0}
!10 = !{!"int", !11, i64 0}
!11 = !{!"omnipotent char", !12, i64 0}
!12 = !{!"Simple C/C++ TBAA"}
*/
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(115124, 7344);
}

// shader_hash: d9c0538b37e05e7ddbcd3a4d9120d287
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 90
// float_instruction_count: 14
// texture_load_instructions: 5
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: d9c0538b37e05e7ddbcd3a4d9120d287.pdb
; shader hash: d9c0538b37e05e7ddbcd3a4d9120d287
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 4 }
;   output 1 depends on inputs: { 0, 4 }
;   output 2 depends on inputs: { 0, 4 }
;   output 3 depends on inputs: { 0, 4 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 4 }
;   output 13 depends on inputs: { 4 }
;   output 14 depends on inputs: { 4 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @light_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 524, i32 28 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=28>
  %12 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = extractvalue %dx.types.ResRet.f32 %19, 2
  %23 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %4, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %24 = extractvalue %dx.types.ResRet.f32 %23, 0
  %25 = extractvalue %dx.types.ResRet.f32 %23, 1
  %26 = extractvalue %dx.types.ResRet.f32 %23, 2
  %27 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %4, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %28 = extractvalue %dx.types.ResRet.f32 %27, 0
  %29 = extractvalue %dx.types.ResRet.f32 %27, 1
  %30 = extractvalue %dx.types.ResRet.f32 %27, 2
  %31 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %4, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %32 = extractvalue %dx.types.ResRet.f32 %31, 0
  %33 = extractvalue %dx.types.ResRet.f32 %31, 1
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 13)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = fmul fast float %35, %24
  %37 = fmul fast float %35, %25
  %38 = fmul fast float %35, %26
  %39 = fadd fast float %36, %16
  %40 = fadd fast float %37, %17
  %41 = fadd fast float %38, %18
  %42 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %43 = extractvalue %dx.types.CBufRet.f32 %42, 0
  %44 = extractvalue %dx.types.CBufRet.f32 %42, 1
  %45 = extractvalue %dx.types.CBufRet.f32 %42, 2
  %46 = extractvalue %dx.types.CBufRet.f32 %42, 3
  %47 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %48 = extractvalue %dx.types.CBufRet.f32 %47, 0
  %49 = extractvalue %dx.types.CBufRet.f32 %47, 1
  %50 = extractvalue %dx.types.CBufRet.f32 %47, 2
  %51 = extractvalue %dx.types.CBufRet.f32 %47, 3
  %52 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %53 = extractvalue %dx.types.CBufRet.f32 %52, 0
  %54 = extractvalue %dx.types.CBufRet.f32 %52, 1
  %55 = extractvalue %dx.types.CBufRet.f32 %52, 2
  %56 = extractvalue %dx.types.CBufRet.f32 %52, 3
  %57 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %58 = extractvalue %dx.types.CBufRet.f32 %57, 0
  %59 = extractvalue %dx.types.CBufRet.f32 %57, 1
  %60 = extractvalue %dx.types.CBufRet.f32 %57, 2
  %61 = extractvalue %dx.types.CBufRet.f32 %57, 3
  %62 = fmul fast float %39, %43
  %63 = call float @dx.op.tertiary.f32(i32 46, float %48, float %40, float %62)  ; FMad(a,b,c)
  %64 = call float @dx.op.tertiary.f32(i32 46, float %53, float %41, float %63)  ; FMad(a,b,c)
  %65 = fadd fast float %64, %58
  %66 = fmul fast float %39, %44
  %67 = call float @dx.op.tertiary.f32(i32 46, float %49, float %40, float %66)  ; FMad(a,b,c)
  %68 = call float @dx.op.tertiary.f32(i32 46, float %54, float %41, float %67)  ; FMad(a,b,c)
  %69 = fadd fast float %68, %59
  %70 = fmul fast float %39, %45
  %71 = call float @dx.op.tertiary.f32(i32 46, float %50, float %40, float %70)  ; FMad(a,b,c)
  %72 = call float @dx.op.tertiary.f32(i32 46, float %55, float %41, float %71)  ; FMad(a,b,c)
  %73 = fadd fast float %72, %60
  %74 = fmul fast float %39, %46
  %75 = call float @dx.op.tertiary.f32(i32 46, float %51, float %40, float %74)  ; FMad(a,b,c)
  %76 = call float @dx.op.tertiary.f32(i32 46, float %56, float %41, float %75)  ; FMad(a,b,c)
  %77 = fadd fast float %76, %61
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %65)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %69)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %73)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %77)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %28)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %29)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %30)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %32)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %33)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %22)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{[7 x i32] [i32 5, i32 15, i32 895, i32 0, i32 0, i32 0, i32 28687]}
!7 = !{void ()* @light_vs, !"light_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, !12}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(122468, 5508);
}

// shader_hash: f27939a6f16349e903dc9af6129dfec6
// input_parameters: 4
// output_parameters: 1
// instruction_count: 5
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: f27939a6f16349e903dc9af6129dfec6.pdb
; shader hash: f27939a6f16349e903dc9af6129dfec6
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @light_ps() {
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #0

attributes #0 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @light_ps, !"light_ps", !5, null, !17}
!5 = !{!6, !14, null}
!6 = !{!7, !9, !10, !12}
!7 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!8 = !{i32 0}
!9 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!10 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!11 = !{i32 1}
!12 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !13, i8 2, i32 1, i8 3, i32 3, i8 0, null}
!13 = !{i32 2}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !8, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 8388864}
*/
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(127976, 3156);
}

// shader_hash: 4e1239e66cc230f1aaecc7e547156a48
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 5
// instruction_count: 74
// float_instruction_count: 8
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
; ATTRIBUTE                3   xyzw        4     NONE   float   xyzw
;
; shader debug name: 4e1239e66cc230f1aaecc7e547156a48.pdb
; shader hash: 4e1239e66cc230f1aaecc7e547156a48
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
; ATTRIBUTE                3                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 20
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;   output 16 depends on inputs: { 0 }
;   output 17 depends on inputs: { 0 }
;   output 18 depends on inputs: { 0 }
;   output 19 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @plane_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 2
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %27 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.f32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.f32 %27, 1
  %30 = extractvalue %dx.types.CBufRet.f32 %27, 2
  %31 = extractvalue %dx.types.CBufRet.f32 %27, 3
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 0
  %34 = extractvalue %dx.types.CBufRet.f32 %32, 1
  %35 = extractvalue %dx.types.CBufRet.f32 %32, 2
  %36 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = extractvalue %dx.types.CBufRet.f32 %37, 1
  %40 = extractvalue %dx.types.CBufRet.f32 %37, 2
  %41 = extractvalue %dx.types.CBufRet.f32 %37, 3
  %42 = fmul fast float %23, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %38
  %46 = fmul fast float %24, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %34, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %39
  %50 = fmul fast float %25, %12
  %51 = call float @dx.op.tertiary.f32(i32 46, float %30, float %13, float %50)  ; FMad(a,b,c)
  %52 = call float @dx.op.tertiary.f32(i32 46, float %35, float %14, float %51)  ; FMad(a,b,c)
  %53 = fadd fast float %52, %40
  %54 = fmul fast float %26, %12
  %55 = call float @dx.op.tertiary.f32(i32 46, float %31, float %13, float %54)  ; FMad(a,b,c)
  %56 = call float @dx.op.tertiary.f32(i32 46, float %36, float %14, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %41
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 1, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 4, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{[7 x i32] [i32 5, i32 20, i32 1012607, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @plane_vs, !"plane_vs", !8, !3, !26}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22, !24}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 4, !"ATTRIBUTE", i8 9, i8 0, !25, i8 2, i32 1, i8 4, i32 4, i8 0, !16}
!25 = !{i32 3}
!26 = !{i32 0, i64 1082130688}
*/
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(131132, 5528);
}

// shader_hash: 669b63a697e3a5755aabebde6fb866ca
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 5
// output_parameters: 1
// instruction_count: 105
// dynamic_flow_control_count: 2
// float_instruction_count: 40
// int_instruction_count: 4
// texture_load_instructions: 5
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
; ATTRIBUTE                3   xyzw        4     NONE   float   xy w
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 669b63a697e3a5755aabebde6fb866ca.pdb
; shader hash: 669b63a697e3a5755aabebde6fb866ca
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
; ATTRIBUTE                3                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 20, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 12, 13, 14, 16, 17, 19 }
;   output 1 depends on inputs: { 12, 13, 14, 16, 17, 19 }
;   output 2 depends on inputs: { 12, 13, 14, 16, 17, 19 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @plane_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %13 = extractvalue %dx.types.CBufRet.i32 %9, 1
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 524, i32 28 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=28>
  %16 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.i32 %16, 0
  %18 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %17, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %19 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %18, %dx.types.ResourceProperties { i32 2, i32 261 })  ; AnnotateHandle(res,props)  resource: Texture2D<U32>
  %20 = extractvalue %dx.types.CBufRet.i32 %16, 1
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %20, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 2, i32 261 })  ; AnnotateHandle(res,props)  resource: Texture2D<U32>
  %23 = extractvalue %dx.types.CBufRet.i32 %16, 2
  %24 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %23, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %25 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %24, %dx.types.ResourceProperties { i32 12, i32 4 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=4>
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 12)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = fdiv fast float %3, %5
  %30 = fdiv fast float %4, %5
  %31 = fmul fast float %29, 5.000000e-01
  %32 = fmul fast float %30, 5.000000e-01
  %33 = fadd fast float %31, 5.000000e-01
  %34 = fsub fast float 5.000000e-01, %32
  %35 = fmul fast float %27, 6.250000e-02
  %36 = fmul fast float %35, %33
  %37 = fmul fast float %28, 6.250000e-02
  %38 = fmul fast float %37, %34
  %39 = call float @dx.op.unary.f32(i32 27, float %36)  ; Round_ni(value)
  %40 = call float @dx.op.unary.f32(i32 27, float %38)  ; Round_ni(value)
  %41 = fptoui float %39 to i32
  %42 = fptoui float %40 to i32
  %43 = call %dx.types.ResRet.i32 @dx.op.textureLoad.i32(i32 66, %dx.types.Handle %19, i32 0, i32 %41, i32 %42, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %44 = extractvalue %dx.types.ResRet.i32 %43, 0
  %45 = call %dx.types.ResRet.i32 @dx.op.textureLoad.i32(i32 66, %dx.types.Handle %22, i32 0, i32 %41, i32 %42, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %46 = extractvalue %dx.types.ResRet.i32 %45, 0
  %47 = icmp eq i32 %44, 0
  br i1 %47, label %50, label %48

; <label>:48                                      ; preds = %0
  br label %54

; <label>:49                                      ; preds = %54
  br label %50

; <label>:50                                      ; preds = %49, %0
  %51 = phi float [ 0x3F947AE140000000, %0 ], [ %96, %49 ]
  %52 = phi float [ 0x3F947AE140000000, %0 ], [ %97, %49 ]
  %53 = phi float [ 0x3F947AE140000000, %0 ], [ %98, %49 ]
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %51)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %52)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void

; <label>:54                                      ; preds = %54, %48
  %55 = phi float [ %96, %54 ], [ 0x3F947AE140000000, %48 ]
  %56 = phi float [ %97, %54 ], [ 0x3F947AE140000000, %48 ]
  %57 = phi float [ %98, %54 ], [ 0x3F947AE140000000, %48 ]
  %58 = phi i32 [ %99, %54 ], [ 0, %48 ]
  %59 = add i32 %58, %46
  %60 = call %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32 139, %dx.types.Handle %25, i32 %59, i32 0, i8 1, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %61 = extractvalue %dx.types.ResRet.i32 %60, 0
  %62 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %15, i32 %61, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %63 = extractvalue %dx.types.ResRet.f32 %62, 0
  %64 = extractvalue %dx.types.ResRet.f32 %62, 1
  %65 = extractvalue %dx.types.ResRet.f32 %62, 2
  %66 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %15, i32 %61, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %67 = extractvalue %dx.types.ResRet.f32 %66, 0
  %68 = extractvalue %dx.types.ResRet.f32 %66, 1
  %69 = extractvalue %dx.types.ResRet.f32 %66, 2
  %70 = fsub fast float %63, %6
  %71 = fsub fast float %64, %7
  %72 = fsub fast float %65, %8
  %73 = fmul fast float %70, %70
  %74 = fmul fast float %71, %71
  %75 = fadd fast float %73, %74
  %76 = fmul fast float %72, %72
  %77 = fadd fast float %75, %76
  %78 = call float @dx.op.unary.f32(i32 24, float %77)  ; Sqrt(value)
  %79 = call float @dx.op.binary.f32(i32 35, float 0x3F50624DE0000000, float %78)  ; FMax(a,b)
  %80 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 13)  ; CBufferLoadLegacy(handle,regIndex)
  %81 = extractvalue %dx.types.CBufRet.f32 %80, 0
  %82 = fdiv fast float %79, %81
  %83 = fmul fast float %82, %82
  %84 = fmul fast float %83, %83
  %85 = fsub fast float 1.000000e+00, %84
  %86 = call float @dx.op.unary.f32(i32 7, float %85)  ; Saturate(value)
  %87 = fmul fast float %79, %79
  %88 = fdiv fast float %86, %87
  %89 = fmul fast float %88, %67
  %90 = fmul fast float %88, %68
  %91 = fmul fast float %88, %69
  %92 = extractvalue %dx.types.CBufRet.f32 %80, 1
  %93 = fmul fast float %89, %92
  %94 = fmul fast float %90, %92
  %95 = fmul fast float %91, %92
  %96 = fadd fast float %93, %55
  %97 = fadd fast float %94, %56
  %98 = fadd fast float %95, %57
  %99 = add nuw i32 %58, 1
  %100 = icmp eq i32 %99, %44
  br i1 %100, label %49, label %54
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.textureLoad.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.rawBufferLoad.i32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{[22 x i32] [i32 20, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 0, i32 7]}
!7 = !{void ()* @plane_ps, !"plane_ps", !8, !3, !24}
!8 = !{!9, !21, null}
!9 = !{!10, !12, !13, !15, !18}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!13 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !14, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!14 = !{i32 1}
!15 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !16, i8 2, i32 1, i8 3, i32 3, i8 0, !17}
!16 = !{i32 2}
!17 = !{i32 3, i32 7}
!18 = !{i32 4, !"ATTRIBUTE", i8 9, i8 0, !19, i8 2, i32 1, i8 4, i32 4, i8 0, !20}
!19 = !{i32 3}
!20 = !{i32 3, i32 11}
!21 = !{!22}
!22 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !23}
!23 = !{i32 3, i32 15}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(136660, 5944);
}

// shader_hash: 3c7eef96db8f3d7630188b073357bb1d
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: 3c7eef96db8f3d7630188b073357bb1d.pdb
; shader hash: 3c7eef96db8f3d7630188b073357bb1d
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @debug_vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @debug_vs, !"debug_vs", !5, null, !16}
!5 = !{!6, !11, null}
!6 = !{!7, !10}
!7 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !8, i8 0, i32 1, i8 1, i32 0, i8 0, !9}
!8 = !{i32 0}
!9 = !{i32 3, i32 1}
!10 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !8, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 0, i64 8388864}
*/
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(142604, 3268);
}

// shader_hash: bafb272ddc9366ca12cc1f13caf82d43
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 44
// float_instruction_count: 11
// texture_normal_instructions: 1
// texture_load_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: bafb272ddc9366ca12cc1f13caf82d43.pdb
; shader hash: bafb272ddc9366ca12cc1f13caf82d43
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint lights;                              ; Offset:    4
;           uint vertices;                            ; Offset:    8
;           uint heatmap_texture;                     ; Offset:   12
;           uint light_counts_texture;                ; Offset:   16
;           uint light_offsets_texture;               ; Offset:   20
;           uint light_indices;                       ; Offset:   24
;           uint light_indices_count;                 ; Offset:   28
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    32
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.i32 = type { i32, i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32, i32, i32, i32, i32, i32 }

define void @debug_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 32 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 356 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 4098, i32 261 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<U32>
  %13 = extractvalue %dx.types.CBufRet.i32 %5, 3
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 2, i32 777 })  ; AnnotateHandle(res,props)  resource: Texture2D<3xF32>
  %16 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %17 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %16, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %18 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 12)  ; CBufferLoadLegacy(handle,regIndex)
  %19 = extractvalue %dx.types.CBufRet.f32 %18, 0
  %20 = extractvalue %dx.types.CBufRet.f32 %18, 1
  %21 = fmul fast float %3, 6.250000e-02
  %22 = fmul fast float %21, %19
  %23 = fmul fast float %4, 6.250000e-02
  %24 = fmul fast float %23, %20
  %25 = call float @dx.op.unary.f32(i32 27, float %22)  ; Round_ni(value)
  %26 = call float @dx.op.unary.f32(i32 27, float %24)  ; Round_ni(value)
  %27 = fptoui float %25 to i32
  %28 = fptoui float %26 to i32
  %29 = call %dx.types.ResRet.i32 @dx.op.textureLoad.i32(i32 66, %dx.types.Handle %12, i32 undef, i32 %27, i32 %28, i32 undef, i32 undef, i32 undef, i32 undef)  ; TextureLoad(srv,mipLevelOrSampleCount,coord0,coord1,coord2,offset0,offset1,offset2)
  %30 = extractvalue %dx.types.ResRet.i32 %29, 0
  %31 = uitofp i32 %30 to float
  %32 = fmul fast float %31, 0x3FC99999A0000000
  %33 = call float @dx.op.unary.f32(i32 7, float %32)  ; Saturate(value)
  %34 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %15, %dx.types.Handle %17, float %33, float 5.000000e-01, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %35 = extractvalue %dx.types.ResRet.f32 %34, 0
  %36 = extractvalue %dx.types.ResRet.f32 %34, 1
  %37 = extractvalue %dx.types.ResRet.f32 %34, 2
  %38 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 13)  ; CBufferLoadLegacy(handle,regIndex)
  %39 = extractvalue %dx.types.CBufRet.f32 %38, 2
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %35)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %36)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %37)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %39)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.i32 @dx.op.textureLoad.i32(i32, %dx.types.Handle, i32, i32, i32, i32, i32, i32, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 32, null}
!6 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7]}
!7 = !{void ()* @debug_ps, !"debug_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!13 = !{i32 3, i32 3}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 11819548928}
*/
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(145872, 4960);
}

// shader_hash: cc2a934663d49520ba211c3cf848160d
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: cc2a934663d49520ba211c3cf848160d.pdb
; shader hash: cc2a934663d49520ba211c3cf848160d
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_background_bindings
; {
;
;   struct g_background_bindings
;   {
;
;       struct struct.BackgroundBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;       
;       } g_background_bindings;                      ; Offset:    0
;
;   
;   } g_background_bindings;                          ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_background_bindings             cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_background_bindings = type { %struct.BackgroundBindings }
%struct.BackgroundBindings = type { i32, i32, i32 }

define void @background_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 772 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %38 = fmul fast float %23, %12
  %39 = call float @dx.op.tertiary.f32(i32 46, float %27, float %13, float %38)  ; FMad(a,b,c)
  %40 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %39)  ; FMad(a,b,c)
  %41 = fadd fast float %40, %35
  %42 = fmul fast float %24, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %32, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %36
  %46 = fmul fast float %25, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %37
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %41)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_background_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[7 x i32] [i32 5, i32 15, i32 29567, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @background_vs, !"background_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::env_background_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(150832, 5196);
}

// shader_hash: 8a3c862749ec501ff0b8f3a65bc2e6be
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 68
// dynamic_flow_control_count: 1
// float_instruction_count: 32
// int_instruction_count: 2
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 8a3c862749ec501ff0b8f3a65bc2e6be.pdb
; shader hash: 8a3c862749ec501ff0b8f3a65bc2e6be
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_background_bindings
; {
;
;   struct g_background_bindings
;   {
;
;       struct struct.BackgroundBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;       
;       } g_background_bindings;                      ; Offset:    0
;
;   
;   } g_background_bindings;                          ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_background_bindings             cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 12, 13, 14 }
;   output 1 depends on inputs: { 12, 13, 14 }
;   output 2 depends on inputs: { 12, 13, 14 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_background_bindings = type { %struct.BackgroundBindings }
%struct.BackgroundBindings = type { i32, i32, i32 }

define void @background_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 772 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 5, i32 1033 })  ; AnnotateHandle(res,props)  resource: TextureCube<4xF32>
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 1, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %15 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %16 = extractvalue %dx.types.CBufRet.f32 %15, 2
  %17 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %9, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %18 = extractvalue %dx.types.CBufRet.i32 %17, 3
  %19 = add i32 %18, -1
  %20 = uitofp i32 %19 to float
  %21 = fmul fast float %20, %16
  %22 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %12, %dx.types.Handle %14, float %3, float %4, float %5, float undef, i32 undef, i32 undef, i32 undef, float %21)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %23 = extractvalue %dx.types.ResRet.f32 %22, 0
  %24 = extractvalue %dx.types.ResRet.f32 %22, 1
  %25 = extractvalue %dx.types.ResRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.i32 %17, 0
  %27 = icmp eq i32 %26, 0
  br i1 %27, label %56, label %28

; <label>:28                                      ; preds = %0
  %29 = fmul fast float %23, 0x4004147AE0000000
  %30 = fmul fast float %24, 0x4004147AE0000000
  %31 = fmul fast float %25, 0x4004147AE0000000
  %32 = fadd fast float %29, 0x3F9EB851E0000000
  %33 = fadd fast float %30, 0x3F9EB851E0000000
  %34 = fadd fast float %31, 0x3F9EB851E0000000
  %35 = fmul fast float %32, %23
  %36 = fmul fast float %33, %24
  %37 = fmul fast float %34, %25
  %38 = fmul fast float %23, 0x400370A3E0000000
  %39 = fmul fast float %24, 0x400370A3E0000000
  %40 = fmul fast float %25, 0x400370A3E0000000
  %41 = fadd fast float %38, 0x3FE2E147A0000000
  %42 = fadd fast float %39, 0x3FE2E147A0000000
  %43 = fadd fast float %40, 0x3FE2E147A0000000
  %44 = fmul fast float %41, %23
  %45 = fmul fast float %42, %24
  %46 = fmul fast float %43, %25
  %47 = fadd fast float %44, 0x3FC1EB8520000000
  %48 = fadd fast float %45, 0x3FC1EB8520000000
  %49 = fadd fast float %46, 0x3FC1EB8520000000
  %50 = fdiv fast float %35, %47
  %51 = fdiv fast float %36, %48
  %52 = fdiv fast float %37, %49
  %53 = call float @dx.op.unary.f32(i32 7, float %50)  ; Saturate(value)
  %54 = call float @dx.op.unary.f32(i32 7, float %51)  ; Saturate(value)
  %55 = call float @dx.op.unary.f32(i32 7, float %52)  ; Saturate(value)
  br label %56

; <label>:56                                      ; preds = %28, %0
  %57 = phi float [ %53, %28 ], [ %23, %0 ]
  %58 = phi float [ %54, %28 ], [ %24, %0 ]
  %59 = phi float [ %55, %28 ], [ %25, %0 ]
  %60 = extractvalue %dx.types.CBufRet.f32 %15, 1
  %61 = fmul fast float %60, %57
  %62 = fmul fast float %60, %58
  %63 = fmul fast float %60, %59
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %61)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %62)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %63)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_background_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @background_ps, !"background_ps", !8, !3, !21}
!8 = !{!9, !18, null}
!9 = !{!10, !12, !13, !15}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!13 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !14, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!14 = !{i32 1}
!15 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !16, i8 2, i32 1, i8 3, i32 3, i8 0, !17}
!16 = !{i32 2}
!17 = !{i32 3, i32 7}
!18 = !{!19}
!19 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !20}
!20 = !{i32 3, i32 15}
!21 = !{i32 0, i64 3229614336}
*/
auto Shaders::env_background_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(156028, 5104);
}

// shader_hash: 0038661577734484a66656a28dbdc941
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 65
// float_instruction_count: 8
// texture_load_instructions: 2
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xyz         2     NONE   float   xyz 
;
; shader debug name: 0038661577734484a66656a28dbdc941.pdb
; shader hash: 0038661577734484a66656a28dbdc941
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Buffer Definitions:
;
; cbuffer g_model_bindings
; {
;
;   struct g_model_bindings
;   {
;
;       struct struct.ModelBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint lut_texture;                         ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint rad_texture;                         ; Offset:   16
;       
;       } g_model_bindings;                           ; Offset:    0
;
;   
;   } g_model_bindings;                               ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_model_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 11
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 10 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_model_bindings = type { %struct.ModelBindings }
%struct.ModelBindings = type { i32, i32, i32, i32, i32 }

define void @model_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 740 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.f32 %19, 0
  %21 = extractvalue %dx.types.CBufRet.f32 %19, 1
  %22 = extractvalue %dx.types.CBufRet.f32 %19, 2
  %23 = extractvalue %dx.types.CBufRet.f32 %19, 3
  %24 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %25 = extractvalue %dx.types.CBufRet.f32 %24, 0
  %26 = extractvalue %dx.types.CBufRet.f32 %24, 1
  %27 = extractvalue %dx.types.CBufRet.f32 %24, 2
  %28 = extractvalue %dx.types.CBufRet.f32 %24, 3
  %29 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %30 = extractvalue %dx.types.CBufRet.f32 %29, 0
  %31 = extractvalue %dx.types.CBufRet.f32 %29, 1
  %32 = extractvalue %dx.types.CBufRet.f32 %29, 2
  %33 = extractvalue %dx.types.CBufRet.f32 %29, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 2
  %38 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %39 = fmul fast float %20, %12
  %40 = call float @dx.op.tertiary.f32(i32 46, float %25, float %13, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %30, float %14, float %40)  ; FMad(a,b,c)
  %42 = fadd fast float %41, %35
  %43 = fmul fast float %21, %12
  %44 = call float @dx.op.tertiary.f32(i32 46, float %26, float %13, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %44)  ; FMad(a,b,c)
  %46 = fadd fast float %45, %36
  %47 = fmul fast float %22, %12
  %48 = call float @dx.op.tertiary.f32(i32 46, float %27, float %13, float %47)  ; FMad(a,b,c)
  %49 = call float @dx.op.tertiary.f32(i32 46, float %32, float %14, float %48)  ; FMad(a,b,c)
  %50 = fadd fast float %49, %37
  %51 = fmul fast float %23, %12
  %52 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %51)  ; FMad(a,b,c)
  %53 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %52)  ; FMad(a,b,c)
  %54 = fadd fast float %53, %38
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %42)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %46)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %50)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %54)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_model_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{[7 x i32] [i32 5, i32 11, i32 1919, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @model_vs, !"model_vs", !8, !3, !21}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 3, i32 2, i8 0, !18}
!20 = !{i32 1}
!21 = !{i32 0, i64 1082130688}
*/
auto Shaders::env_model_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(161132, 5100);
}

// shader_hash: d9b82a874bca714c1d4129251bfe07fa
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 124
// dynamic_flow_control_count: 1
// float_instruction_count: 66
// int_instruction_count: 1
// texture_normal_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xyz         2     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: d9b82a874bca714c1d4129251bfe07fa.pdb
; shader hash: d9b82a874bca714c1d4129251bfe07fa
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_model_bindings
; {
;
;   struct g_model_bindings
;   {
;
;       struct struct.ModelBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint lut_texture;                         ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint rad_texture;                         ; Offset:   16
;       
;       } g_model_bindings;                           ; Offset:    0
;
;   
;   } g_model_bindings;                               ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_model_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 11, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;   output 1 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;   output 2 depends on inputs: { 4, 5, 6, 8, 9, 10 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_model_bindings = type { %struct.ModelBindings }
%struct.ModelBindings = type { i32, i32, i32, i32, i32 }

define void @model_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 13, i32 740 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %13 = extractvalue %dx.types.CBufRet.i32 %9, 2
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 2, i32 521 })  ; AnnotateHandle(res,props)  resource: Texture2D<2xF32>
  %16 = extractvalue %dx.types.CBufRet.i32 %9, 3
  %17 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %16, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %18 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %17, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %19 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %20 = extractvalue %dx.types.CBufRet.i32 %19, 0
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %20, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %23 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %24 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %23, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %25 = call float @dx.op.dot3.f32(i32 55, float %6, float %7, float %8, float %6, float %7, float %8)  ; Dot3(ax,ay,az,bx,by,bz)
  %26 = call float @dx.op.unary.f32(i32 25, float %25)  ; Rsqrt(value)
  %27 = fmul fast float %26, %6
  %28 = fmul fast float %26, %7
  %29 = fmul fast float %26, %8
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 2
  %34 = fsub fast float %31, %3
  %35 = fsub fast float %32, %4
  %36 = fsub fast float %33, %5
  %37 = call float @dx.op.dot3.f32(i32 55, float %34, float %35, float %36, float %34, float %35, float %36)  ; Dot3(ax,ay,az,bx,by,bz)
  %38 = call float @dx.op.unary.f32(i32 25, float %37)  ; Rsqrt(value)
  %39 = fmul fast float %34, %38
  %40 = fmul fast float %35, %38
  %41 = fmul fast float %36, %38
  %42 = fsub fast float -0.000000e+00, %39
  %43 = fsub fast float -0.000000e+00, %40
  %44 = fsub fast float -0.000000e+00, %41
  %45 = call float @dx.op.dot3.f32(i32 55, float %42, float %43, float %44, float %27, float %28, float %29)  ; Dot3(ax,ay,az,bx,by,bz)
  %46 = fmul fast float %45, 2.000000e+00
  %47 = fmul fast float %46, %27
  %48 = fmul fast float %46, %28
  %49 = fmul fast float %46, %29
  %50 = fsub fast float %42, %47
  %51 = fsub fast float %43, %48
  %52 = fsub fast float %44, %49
  %53 = call float @dx.op.dot3.f32(i32 55, float %27, float %28, float %29, float %39, float %40, float %41)  ; Dot3(ax,ay,az,bx,by,bz)
  %54 = call float @dx.op.unary.f32(i32 7, float %53)  ; Saturate(value)
  %55 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %56 = extractvalue %dx.types.CBufRet.f32 %55, 1
  %57 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %12, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %58 = extractvalue %dx.types.CBufRet.i32 %57, 2
  %59 = uitofp i32 %58 to float
  %60 = fadd fast float %59, -1.000000e+00
  %61 = fmul fast float %60, %56
  %62 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %15, %dx.types.Handle %24, float %54, float %56, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %63 = extractvalue %dx.types.ResRet.f32 %62, 0
  %64 = extractvalue %dx.types.ResRet.f32 %62, 1
  %65 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %18, %dx.types.Handle %24, float %27, float %28, float %29, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %66 = extractvalue %dx.types.ResRet.f32 %65, 0
  %67 = extractvalue %dx.types.ResRet.f32 %65, 1
  %68 = extractvalue %dx.types.ResRet.f32 %65, 2
  %69 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %22, %dx.types.Handle %24, float %50, float %51, float %52, float undef, i32 undef, i32 undef, i32 undef, float %61)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %70 = extractvalue %dx.types.ResRet.f32 %69, 0
  %71 = extractvalue %dx.types.ResRet.f32 %69, 1
  %72 = extractvalue %dx.types.ResRet.f32 %69, 2
  %73 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %12, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %74 = extractvalue %dx.types.CBufRet.f32 %73, 0
  %75 = fmul fast float %74, 0x3FEEB851E0000000
  %76 = fadd fast float %75, 0x3FA47AE140000000
  %77 = fmul fast float %76, %63
  %78 = fadd fast float %77, %64
  %79 = fmul fast float %78, %70
  %80 = fmul fast float %78, %71
  %81 = fmul fast float %78, %72
  %82 = fadd fast float %79, %66
  %83 = fadd fast float %80, %67
  %84 = fadd fast float %81, %68
  %85 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %12, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %86 = extractvalue %dx.types.CBufRet.i32 %85, 3
  %87 = icmp eq i32 %86, 0
  br i1 %87, label %116, label %88

; <label>:88                                      ; preds = %0
  %89 = fmul fast float %82, 0x4004147AE0000000
  %90 = fmul fast float %83, 0x4004147AE0000000
  %91 = fmul fast float %84, 0x4004147AE0000000
  %92 = fadd fast float %89, 0x3F9EB851E0000000
  %93 = fadd fast float %90, 0x3F9EB851E0000000
  %94 = fadd fast float %91, 0x3F9EB851E0000000
  %95 = fmul fast float %92, %82
  %96 = fmul fast float %93, %83
  %97 = fmul fast float %94, %84
  %98 = fmul fast float %82, 0x400370A3E0000000
  %99 = fmul fast float %83, 0x400370A3E0000000
  %100 = fmul fast float %84, 0x400370A3E0000000
  %101 = fadd fast float %98, 0x3FE2E147A0000000
  %102 = fadd fast float %99, 0x3FE2E147A0000000
  %103 = fadd fast float %100, 0x3FE2E147A0000000
  %104 = fmul fast float %101, %82
  %105 = fmul fast float %102, %83
  %106 = fmul fast float %103, %84
  %107 = fadd fast float %104, 0x3FC1EB8520000000
  %108 = fadd fast float %105, 0x3FC1EB8520000000
  %109 = fadd fast float %106, 0x3FC1EB8520000000
  %110 = fdiv fast float %95, %107
  %111 = fdiv fast float %96, %108
  %112 = fdiv fast float %97, %109
  %113 = call float @dx.op.unary.f32(i32 7, float %110)  ; Saturate(value)
  %114 = call float @dx.op.unary.f32(i32 7, float %111)  ; Saturate(value)
  %115 = call float @dx.op.unary.f32(i32 7, float %112)  ; Saturate(value)
  br label %116

; <label>:116                                     ; preds = %88, %0
  %117 = phi float [ %113, %88 ], [ %82, %0 ]
  %118 = phi float [ %114, %88 ], [ %83, %0 ]
  %119 = phi float [ %115, %88 ], [ %84, %0 ]
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %117)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %118)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %119)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_model_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{[13 x i32] [i32 11, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @model_ps, !"model_ps", !8, !3, !19}
!8 = !{!9, !16, null}
!9 = !{!10, !12, !14}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !15, i8 2, i32 1, i8 3, i32 2, i8 0, !13}
!15 = !{i32 1}
!16 = !{!17}
!17 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !18}
!18 = !{i32 3, i32 15}
!19 = !{i32 0, i64 3229614336}
*/
auto Shaders::env_model_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(166232, 5612);
}

// shader_hash: 821eee7a94d6d2a2fa9e82f5c365d7f6
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
; shader debug name: 821eee7a94d6d2a2fa9e82f5c365d7f6.pdb
; shader hash: 821eee7a94d6d2a2fa9e82f5c365d7f6
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Buffer Definitions:
;
; cbuffer g_background_bindings
; {
;
;   struct g_background_bindings
;   {
;
;       struct struct.BackgroundBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint irr_texture;                         ; Offset:    8
;           uint sampler;                             ; Offset:   12
;       
;       } g_background_bindings;                      ; Offset:    0
;
;   
;   } g_background_bindings;                          ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_background_bindings             cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 15
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;   output 12 depends on inputs: { 0 }
;   output 13 depends on inputs: { 0 }
;   output 14 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_background_bindings = type { %struct.BackgroundBindings }
%struct.BackgroundBindings = type { i32, i32, i32, i32 }

define void @background_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %26 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %27 = extractvalue %dx.types.CBufRet.f32 %26, 0
  %28 = extractvalue %dx.types.CBufRet.f32 %26, 1
  %29 = extractvalue %dx.types.CBufRet.f32 %26, 3
  %30 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %31 = extractvalue %dx.types.CBufRet.f32 %30, 0
  %32 = extractvalue %dx.types.CBufRet.f32 %30, 1
  %33 = extractvalue %dx.types.CBufRet.f32 %30, 3
  %34 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %35 = extractvalue %dx.types.CBufRet.f32 %34, 0
  %36 = extractvalue %dx.types.CBufRet.f32 %34, 1
  %37 = extractvalue %dx.types.CBufRet.f32 %34, 3
  %38 = fmul fast float %23, %12
  %39 = call float @dx.op.tertiary.f32(i32 46, float %27, float %13, float %38)  ; FMad(a,b,c)
  %40 = call float @dx.op.tertiary.f32(i32 46, float %31, float %14, float %39)  ; FMad(a,b,c)
  %41 = fadd fast float %40, %35
  %42 = fmul fast float %24, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %32, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %36
  %46 = fmul fast float %25, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %37
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %41)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 0, float %12)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 1, float %13)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 3, i32 0, i8 2, float %14)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_background_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[7 x i32] [i32 5, i32 15, i32 29567, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @background_vs, !"background_vs", !8, !3, !24}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19, !22}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !23, i8 2, i32 1, i8 3, i32 3, i8 0, !18}
!23 = !{i32 2}
!24 = !{i32 0, i64 1082130688}
*/
auto Shaders::text_background_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(171844, 5224);
}

// shader_hash: a3925396d8411959d700b45c4725ed70
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 26
// float_instruction_count: 5
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float       
; ATTRIBUTE                2   xyz         3     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: a3925396d8411959d700b45c4725ed70.pdb
; shader hash: a3925396d8411959d700b45c4725ed70
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
; ATTRIBUTE                2                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_background_bindings
; {
;
;   struct g_background_bindings
;   {
;
;       struct struct.BackgroundBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint irr_texture;                         ; Offset:    8
;           uint sampler;                             ; Offset:   12
;       
;       } g_background_bindings;                      ; Offset:    0
;
;   
;   } g_background_bindings;                          ; Offset:    0 Size:    16
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_background_bindings             cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 15, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 12, 13, 14 }
;   output 1 depends on inputs: { 12, 13, 14 }
;   output 2 depends on inputs: { 12, 13, 14 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_background_bindings = type { %struct.BackgroundBindings }
%struct.BackgroundBindings = type { i32, i32, i32, i32 }

define void @background_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 16 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 2
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %10 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %11 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %10, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %12 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %11, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %13 = call float @dx.op.dot3.f32(i32 55, float %3, float %4, float %5, float %3, float %4, float %5)  ; Dot3(ax,ay,az,bx,by,bz)
  %14 = call float @dx.op.unary.f32(i32 25, float %13)  ; Rsqrt(value)
  %15 = fmul fast float %14, %3
  %16 = fmul fast float %14, %4
  %17 = fmul fast float %14, %5
  %18 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %9, %dx.types.Handle %12, float %15, float %16, float %17, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %19 = extractvalue %dx.types.ResRet.f32 %18, 0
  %20 = extractvalue %dx.types.ResRet.f32 %18, 1
  %21 = extractvalue %dx.types.ResRet.f32 %18, 2
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %19)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_background_bindings* undef, !"", i32 0, i32 0, i32 1, i32 16, null}
!6 = !{[17 x i32] [i32 15, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @background_ps, !"background_ps", !8, !3, !21}
!8 = !{!9, !18, null}
!9 = !{!10, !12, !13, !15}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!13 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !14, i8 2, i32 1, i8 2, i32 2, i8 0, null}
!14 = !{i32 1}
!15 = !{i32 3, !"ATTRIBUTE", i8 9, i8 0, !16, i8 2, i32 1, i8 3, i32 3, i8 0, !17}
!16 = !{i32 2}
!17 = !{i32 3, i32 7}
!18 = !{!19}
!19 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !20}
!20 = !{i32 3, i32 15}
!21 = !{i32 0, i64 3229614336}
*/
auto Shaders::text_background_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(177068, 4760);
}

// shader_hash: a53097e80b634f80e3bfd6140623a0f4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 97
// float_instruction_count: 14
// int_instruction_count: 1
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
;
; shader debug name: a53097e80b634f80e3bfd6140623a0f4.pdb
; shader hash: a53097e80b634f80e3bfd6140623a0f4
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
; cbuffer g_glyph_bindings
; {
;
;   struct g_glyph_bindings
;   {
;
;       struct struct.GlyphBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint instances;                           ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint sampler;                             ; Offset:   16
;           uint base_vertex;                         ; Offset:   20
;           uint instance_id;                         ; Offset:   24
;       
;       } g_glyph_bindings;                           ; Offset:    0
;
;   
;   } g_glyph_bindings;                               ; Offset:    0 Size:    28
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_glyph_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 7
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_glyph_bindings = type { %struct.GlyphBindings }
%struct.GlyphBindings = type { i32, i32, i32, i32, i32, i32, i32 }

define void @glyph_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 28 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 580 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = extractvalue %dx.types.CBufRet.i32 %4, 2
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 524, i32 12 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=12>
  %14 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %15 = extractvalue %dx.types.CBufRet.i32 %14, 1
  %16 = add i32 %15, %3
  %17 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %16, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %18 = extractvalue %dx.types.ResRet.f32 %17, 0
  %19 = extractvalue %dx.types.ResRet.f32 %17, 1
  %20 = extractvalue %dx.types.ResRet.f32 %17, 2
  %21 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %16, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %22 = extractvalue %dx.types.ResRet.f32 %21, 0
  %23 = extractvalue %dx.types.ResRet.f32 %21, 1
  %24 = extractvalue %dx.types.ResRet.f32 %21, 2
  %25 = extractvalue %dx.types.CBufRet.i32 %14, 2
  %26 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %13, i32 %25, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %27 = extractvalue %dx.types.ResRet.f32 %26, 0
  %28 = extractvalue %dx.types.ResRet.f32 %26, 1
  %29 = extractvalue %dx.types.ResRet.f32 %26, 2
  %30 = fadd fast float %27, %18
  %31 = fadd fast float %28, %19
  %32 = fadd fast float %29, %20
  %33 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %34 = extractvalue %dx.types.CBufRet.f32 %33, 0
  %35 = extractvalue %dx.types.CBufRet.f32 %33, 1
  %36 = extractvalue %dx.types.CBufRet.f32 %33, 2
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 5)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = extractvalue %dx.types.CBufRet.f32 %37, 1
  %40 = extractvalue %dx.types.CBufRet.f32 %37, 2
  %41 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 6)  ; CBufferLoadLegacy(handle,regIndex)
  %42 = extractvalue %dx.types.CBufRet.f32 %41, 0
  %43 = extractvalue %dx.types.CBufRet.f32 %41, 1
  %44 = extractvalue %dx.types.CBufRet.f32 %41, 2
  %45 = fmul fast float %34, %22
  %46 = call float @dx.op.tertiary.f32(i32 46, float %38, float %23, float %45)  ; FMad(a,b,c)
  %47 = call float @dx.op.tertiary.f32(i32 46, float %42, float %24, float %46)  ; FMad(a,b,c)
  %48 = fmul fast float %35, %22
  %49 = call float @dx.op.tertiary.f32(i32 46, float %39, float %23, float %48)  ; FMad(a,b,c)
  %50 = call float @dx.op.tertiary.f32(i32 46, float %43, float %24, float %49)  ; FMad(a,b,c)
  %51 = fmul fast float %36, %22
  %52 = call float @dx.op.tertiary.f32(i32 46, float %40, float %23, float %51)  ; FMad(a,b,c)
  %53 = call float @dx.op.tertiary.f32(i32 46, float %44, float %24, float %52)  ; FMad(a,b,c)
  %54 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %55 = extractvalue %dx.types.CBufRet.f32 %54, 0
  %56 = extractvalue %dx.types.CBufRet.f32 %54, 1
  %57 = extractvalue %dx.types.CBufRet.f32 %54, 2
  %58 = extractvalue %dx.types.CBufRet.f32 %54, 3
  %59 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %60 = extractvalue %dx.types.CBufRet.f32 %59, 0
  %61 = extractvalue %dx.types.CBufRet.f32 %59, 1
  %62 = extractvalue %dx.types.CBufRet.f32 %59, 2
  %63 = extractvalue %dx.types.CBufRet.f32 %59, 3
  %64 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %65 = extractvalue %dx.types.CBufRet.f32 %64, 0
  %66 = extractvalue %dx.types.CBufRet.f32 %64, 1
  %67 = extractvalue %dx.types.CBufRet.f32 %64, 2
  %68 = extractvalue %dx.types.CBufRet.f32 %64, 3
  %69 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %70 = extractvalue %dx.types.CBufRet.f32 %69, 0
  %71 = extractvalue %dx.types.CBufRet.f32 %69, 1
  %72 = extractvalue %dx.types.CBufRet.f32 %69, 2
  %73 = extractvalue %dx.types.CBufRet.f32 %69, 3
  %74 = fmul fast float %55, %30
  %75 = call float @dx.op.tertiary.f32(i32 46, float %60, float %31, float %74)  ; FMad(a,b,c)
  %76 = call float @dx.op.tertiary.f32(i32 46, float %65, float %32, float %75)  ; FMad(a,b,c)
  %77 = fadd fast float %76, %70
  %78 = fmul fast float %56, %30
  %79 = call float @dx.op.tertiary.f32(i32 46, float %61, float %31, float %78)  ; FMad(a,b,c)
  %80 = call float @dx.op.tertiary.f32(i32 46, float %66, float %32, float %79)  ; FMad(a,b,c)
  %81 = fadd fast float %80, %71
  %82 = fmul fast float %57, %30
  %83 = call float @dx.op.tertiary.f32(i32 46, float %62, float %31, float %82)  ; FMad(a,b,c)
  %84 = call float @dx.op.tertiary.f32(i32 46, float %67, float %32, float %83)  ; FMad(a,b,c)
  %85 = fadd fast float %84, %72
  %86 = fmul fast float %58, %30
  %87 = call float @dx.op.tertiary.f32(i32 46, float %63, float %31, float %86)  ; FMad(a,b,c)
  %88 = call float @dx.op.tertiary.f32(i32 46, float %68, float %32, float %87)  ; FMad(a,b,c)
  %89 = fadd fast float %88, %73
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %77)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %81)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %85)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %89)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %47)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %50)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_glyph_bindings* undef, !"", i32 0, i32 0, i32 1, i32 28, null}
!6 = !{[7 x i32] [i32 5, i32 7, i32 127, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @glyph_vs, !"glyph_vs", !8, !3, !19}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 0, i64 1082130688}
*/
auto Shaders::text_glyph_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(181828, 5256);
}

// shader_hash: 9901d99d4259873d667c147a01fa27e1
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 37
// float_instruction_count: 8
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 9901d99d4259873d667c147a01fa27e1.pdb
; shader hash: 9901d99d4259873d667c147a01fa27e1
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_glyph_bindings
; {
;
;   struct g_glyph_bindings
;   {
;
;       struct struct.GlyphBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint instances;                           ; Offset:    8
;           uint irr_texture;                         ; Offset:   12
;           uint sampler;                             ; Offset:   16
;           uint base_vertex;                         ; Offset:   20
;           uint instance_id;                         ; Offset:   24
;       
;       } g_glyph_bindings;                           ; Offset:    0
;
;   
;   } g_glyph_bindings;                               ; Offset:    0 Size:    28
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_glyph_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 7, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5, 6 }
;   output 1 depends on inputs: { 4, 5, 6 }
;   output 2 depends on inputs: { 4, 5, 6 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_glyph_bindings = type { %struct.GlyphBindings }
%struct.GlyphBindings = type { i32, i32, i32, i32, i32, i32, i32 }

define void @glyph_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 28 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %7 = extractvalue %dx.types.CBufRet.i32 %6, 0
  %8 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %7, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %9 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %8, %dx.types.ResourceProperties { i32 13, i32 580 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %10 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %11 = extractvalue %dx.types.CBufRet.i32 %10, 0
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %14 = extractvalue %dx.types.CBufRet.i32 %6, 3
  %15 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %14, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %16 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %15, %dx.types.ResourceProperties { i32 5, i32 777 })  ; AnnotateHandle(res,props)  resource: TextureCube<3xF32>
  %17 = call float @dx.op.dot3.f32(i32 55, float %3, float %4, float %5, float %3, float %4, float %5)  ; Dot3(ax,ay,az,bx,by,bz)
  %18 = call float @dx.op.unary.f32(i32 25, float %17)  ; Rsqrt(value)
  %19 = fmul fast float %18, %3
  %20 = fmul fast float %18, %4
  %21 = fmul fast float %18, %5
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %9, i32 8)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %16, %dx.types.Handle %13, float %19, float %20, float %21, float undef, i32 undef, i32 undef, i32 undef, float 0.000000e+00)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %27 = extractvalue %dx.types.ResRet.f32 %26, 0
  %28 = extractvalue %dx.types.ResRet.f32 %26, 1
  %29 = extractvalue %dx.types.ResRet.f32 %26, 2
  %30 = fmul fast float %27, %23
  %31 = fmul fast float %28, %24
  %32 = fmul fast float %29, %25
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %30)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %31)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %32)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.dot3.f32(i32, float, float, float, float, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_glyph_bindings* undef, !"", i32 0, i32 0, i32 1, i32 28, null}
!6 = !{[9 x i32] [i32 7, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7, i32 7]}
!7 = !{void ()* @glyph_ps, !"glyph_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !13}
!13 = !{i32 3, i32 7}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 3229614336}
*/
auto Shaders::text_glyph_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(187084, 4752);
}

// shader_hash: 8de171fd593e20e89dffec38a71cb538
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 95
// float_instruction_count: 12
// texture_load_instructions: 5
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint   x   
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
;
; shader debug name: 8de171fd593e20e89dffec38a71cb538.pdb
; shader hash: 8de171fd593e20e89dffec38a71cb538
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Buffer Definitions:
;
; cbuffer g_scene_bindings
; {
;
;   struct g_scene_bindings
;   {
;
;       struct struct.SceneBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint instances;                           ; Offset:    8
;       
;       } g_scene_bindings;                           ; Offset:    0
;
;   
;   } g_scene_bindings;                               ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_scene_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 4 }
;   output 1 depends on inputs: { 0, 4 }
;   output 2 depends on inputs: { 0, 4 }
;   output 3 depends on inputs: { 0, 4 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_scene_bindings = type { %struct.SceneBindings }
%struct.SceneBindings = type { i32, i32, i32 }

define void @scene_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 772 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %12 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %13 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %12, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %14 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %13, %dx.types.ResourceProperties { i32 524, i32 64 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=64>
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %11, i32 %4, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 0, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = extractvalue %dx.types.ResRet.f32 %19, 2
  %23 = extractvalue %dx.types.ResRet.f32 %19, 3
  %24 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 16, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %25 = extractvalue %dx.types.ResRet.f32 %24, 0
  %26 = extractvalue %dx.types.ResRet.f32 %24, 1
  %27 = extractvalue %dx.types.ResRet.f32 %24, 2
  %28 = extractvalue %dx.types.ResRet.f32 %24, 3
  %29 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 32, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %30 = extractvalue %dx.types.ResRet.f32 %29, 0
  %31 = extractvalue %dx.types.ResRet.f32 %29, 1
  %32 = extractvalue %dx.types.ResRet.f32 %29, 2
  %33 = extractvalue %dx.types.ResRet.f32 %29, 3
  %34 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %14, i32 %3, i32 48, i8 15, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %35 = extractvalue %dx.types.ResRet.f32 %34, 0
  %36 = extractvalue %dx.types.ResRet.f32 %34, 1
  %37 = extractvalue %dx.types.ResRet.f32 %34, 2
  %38 = extractvalue %dx.types.ResRet.f32 %34, 3
  %39 = fmul fast float %20, %16
  %40 = call float @dx.op.tertiary.f32(i32 46, float %25, float %17, float %39)  ; FMad(a,b,c)
  %41 = call float @dx.op.tertiary.f32(i32 46, float %30, float %18, float %40)  ; FMad(a,b,c)
  %42 = fadd fast float %41, %35
  %43 = fmul fast float %21, %16
  %44 = call float @dx.op.tertiary.f32(i32 46, float %26, float %17, float %43)  ; FMad(a,b,c)
  %45 = call float @dx.op.tertiary.f32(i32 46, float %31, float %18, float %44)  ; FMad(a,b,c)
  %46 = fadd fast float %45, %36
  %47 = fmul fast float %22, %16
  %48 = call float @dx.op.tertiary.f32(i32 46, float %27, float %17, float %47)  ; FMad(a,b,c)
  %49 = call float @dx.op.tertiary.f32(i32 46, float %32, float %18, float %48)  ; FMad(a,b,c)
  %50 = fadd fast float %49, %37
  %51 = fmul fast float %23, %16
  %52 = call float @dx.op.tertiary.f32(i32 46, float %28, float %17, float %51)  ; FMad(a,b,c)
  %53 = call float @dx.op.tertiary.f32(i32 46, float %33, float %18, float %52)  ; FMad(a,b,c)
  %54 = fadd fast float %53, %38
  %55 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %56 = extractvalue %dx.types.CBufRet.f32 %55, 0
  %57 = extractvalue %dx.types.CBufRet.f32 %55, 1
  %58 = extractvalue %dx.types.CBufRet.f32 %55, 2
  %59 = extractvalue %dx.types.CBufRet.f32 %55, 3
  %60 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %61 = extractvalue %dx.types.CBufRet.f32 %60, 0
  %62 = extractvalue %dx.types.CBufRet.f32 %60, 1
  %63 = extractvalue %dx.types.CBufRet.f32 %60, 2
  %64 = extractvalue %dx.types.CBufRet.f32 %60, 3
  %65 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %66 = extractvalue %dx.types.CBufRet.f32 %65, 0
  %67 = extractvalue %dx.types.CBufRet.f32 %65, 1
  %68 = extractvalue %dx.types.CBufRet.f32 %65, 2
  %69 = extractvalue %dx.types.CBufRet.f32 %65, 3
  %70 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %71 = extractvalue %dx.types.CBufRet.f32 %70, 0
  %72 = extractvalue %dx.types.CBufRet.f32 %70, 1
  %73 = extractvalue %dx.types.CBufRet.f32 %70, 2
  %74 = extractvalue %dx.types.CBufRet.f32 %70, 3
  %75 = fmul fast float %56, %42
  %76 = call float @dx.op.tertiary.f32(i32 46, float %61, float %46, float %75)  ; FMad(a,b,c)
  %77 = call float @dx.op.tertiary.f32(i32 46, float %66, float %50, float %76)  ; FMad(a,b,c)
  %78 = call float @dx.op.tertiary.f32(i32 46, float %71, float %54, float %77)  ; FMad(a,b,c)
  %79 = fmul fast float %57, %42
  %80 = call float @dx.op.tertiary.f32(i32 46, float %62, float %46, float %79)  ; FMad(a,b,c)
  %81 = call float @dx.op.tertiary.f32(i32 46, float %67, float %50, float %80)  ; FMad(a,b,c)
  %82 = call float @dx.op.tertiary.f32(i32 46, float %72, float %54, float %81)  ; FMad(a,b,c)
  %83 = fmul fast float %58, %42
  %84 = call float @dx.op.tertiary.f32(i32 46, float %63, float %46, float %83)  ; FMad(a,b,c)
  %85 = call float @dx.op.tertiary.f32(i32 46, float %68, float %50, float %84)  ; FMad(a,b,c)
  %86 = call float @dx.op.tertiary.f32(i32 46, float %73, float %54, float %85)  ; FMad(a,b,c)
  %87 = fmul fast float %59, %42
  %88 = call float @dx.op.tertiary.f32(i32 46, float %64, float %46, float %87)  ; FMad(a,b,c)
  %89 = call float @dx.op.tertiary.f32(i32 46, float %69, float %50, float %88)  ; FMad(a,b,c)
  %90 = call float @dx.op.tertiary.f32(i32 46, float %74, float %54, float %89)  ; FMad(a,b,c)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %78)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %82)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %86)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %90)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_scene_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[7 x i32] [i32 5, i32 4, i32 15, i32 0, i32 0, i32 0, i32 15]}
!7 = !{void ()* @scene_vs, !"scene_vs", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, !12}
!14 = !{!15}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 1082130688}
*/
auto Shaders::saber_scene_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(191836, 5020);
}

// shader_hash: 7463851ce3601dd2beb39a2322d858c4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 1
// output_parameters: 1
// instruction_count: 22
// float_instruction_count: 6
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 7463851ce3601dd2beb39a2322d858c4.pdb
; shader hash: 7463851ce3601dd2beb39a2322d858c4
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_scene_bindings
; {
;
;   struct g_scene_bindings
;   {
;
;       struct struct.SceneBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint instances;                           ; Offset:    8
;       
;       } g_scene_bindings;                           ; Offset:    0
;
;   
;   } g_scene_bindings;                               ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_scene_bindings                  cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 4, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_scene_bindings = type { %struct.SceneBindings }
%struct.SceneBindings = type { i32, i32, i32 }

define void @scene_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %4 = extractvalue %dx.types.CBufRet.i32 %3, 0
  %5 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %4, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %6 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %5, %dx.types.ResourceProperties { i32 13, i32 772 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %7 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %6, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %8 = extractvalue %dx.types.CBufRet.f32 %7, 0
  %9 = extractvalue %dx.types.CBufRet.f32 %7, 1
  %10 = extractvalue %dx.types.CBufRet.f32 %7, 2
  %11 = extractvalue %dx.types.CBufRet.f32 %7, 3
  %12 = fadd fast float %8, 1.000000e+00
  %13 = fadd fast float %9, 1.000000e+00
  %14 = fadd fast float %10, 1.000000e+00
  %15 = fmul fast float %12, %11
  %16 = fmul fast float %13, %11
  %17 = fmul fast float %14, %11
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %15)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #1

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_scene_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[6 x i32] [i32 4, i32 4, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @scene_ps, !"scene_ps", !8, !3, !15}
!8 = !{!9, !12, null}
!9 = !{!10}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{!13}
!13 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !14}
!14 = !{i32 3, i32 15}
!15 = !{i32 0, i64 1082130688}
*/
auto Shaders::saber_scene_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(196856, 3900);
}

// shader_hash: f4c16de1384addbaa0a4dff7f3ea09cd
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 43
// dynamic_flow_control_count: 2
// float_instruction_count: 16
// int_instruction_count: 2
// texture_normal_instructions: 1
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: f4c16de1384addbaa0a4dff7f3ea09cd.pdb
; shader hash: f4c16de1384addbaa0a4dff7f3ea09cd
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_threshold_bindings
; {
;
;   struct g_threshold_bindings
;   {
;
;       struct struct.ThresholdBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint scene_texture;                       ; Offset:    4
;           uint downsample_texture;                  ; Offset:    8
;           uint texture_width;                       ; Offset:   12
;           uint texture_height;                      ; Offset:   16
;       
;       } g_threshold_bindings;                       ; Offset:    0
;
;   
;   } g_threshold_bindings;                           ; Offset:    0 Size:    20
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_threshold_bindings              cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_threshold_bindings = type { %struct.ThresholdBindings }
%struct.ThresholdBindings = type { i32, i32, i32, i32, i32 }

define void @threshold_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 20 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 3
  %7 = icmp ult i32 %3, %6
  br i1 %7, label %8, label %41

; <label>:8                                       ; preds = %0
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = icmp ult i32 %4, %10
  br i1 %11, label %12, label %41

; <label>:12                                      ; preds = %8
  %13 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %14 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %15 = uitofp i32 %6 to float
  %16 = uitofp i32 %10 to float
  %17 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %18 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %17, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %19 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %14, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %20 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %19, %dx.types.ResourceProperties { i32 4098, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<4xF32>
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %23 = uitofp i32 %3 to float
  %24 = uitofp i32 %4 to float
  %25 = fadd fast float %23, 5.000000e-01
  %26 = fadd fast float %24, 5.000000e-01
  %27 = fdiv fast float 1.000000e+00, %15
  %28 = fdiv fast float 1.000000e+00, %16
  %29 = fmul fast float %27, %25
  %30 = fmul fast float %28, %26
  %31 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %18, %dx.types.Handle %22, float %29, float %30, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %32 = extractvalue %dx.types.ResRet.f32 %31, 0
  %33 = extractvalue %dx.types.ResRet.f32 %31, 1
  %34 = extractvalue %dx.types.ResRet.f32 %31, 2
  %35 = fadd fast float %32, -1.000000e+00
  %36 = fadd fast float %33, -1.000000e+00
  %37 = fadd fast float %34, -1.000000e+00
  %38 = call float @dx.op.binary.f32(i32 35, float 0.000000e+00, float %35)  ; FMax(a,b)
  %39 = call float @dx.op.binary.f32(i32 35, float 0.000000e+00, float %36)  ; FMax(a,b)
  %40 = call float @dx.op.binary.f32(i32 35, float 0.000000e+00, float %37)  ; FMax(a,b)
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %20, i32 %3, i32 %4, i32 undef, float %38, float %39, float %40, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %41

; <label>:41                                      ; preds = %12, %8, %0
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_threshold_bindings* undef, !"", i32 0, i32 0, i32 1, i32 20, null}
!6 = !{void ()* @threshold_cs, !"threshold_cs", null, !3, !7}
!7 = !{i32 0, i64 11819548928, i32 4, !8}
!8 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::saber_threshold_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(200756, 4056);
}

// shader_hash: c63f8b566602e338c3f309ebda3b8444
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 157
// dynamic_flow_control_count: 2
// float_instruction_count: 78
// int_instruction_count: 3
// texture_normal_instructions: 13
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: c63f8b566602e338c3f309ebda3b8444.pdb
; shader hash: c63f8b566602e338c3f309ebda3b8444
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_downsample_bindings
; {
;
;   struct g_downsample_bindings
;   {
;
;       struct struct.DownsampleBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint src_texture;                         ; Offset:    4
;           uint src_texture_level;                   ; Offset:    8
;           uint src_texture_width;                   ; Offset:   12
;           uint src_texture_height;                  ; Offset:   16
;           uint dst_texture;                         ; Offset:   20
;           uint dst_texture_level;                   ; Offset:   24
;           uint dst_texture_width;                   ; Offset:   28
;           uint dst_texture_height;                  ; Offset:   32
;       
;       } g_downsample_bindings;                      ; Offset:    0
;
;   
;   } g_downsample_bindings;                          ; Offset:    0 Size:    36
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_downsample_bindings             cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_downsample_bindings = type { %struct.DownsampleBindings }
%struct.DownsampleBindings = type { i32, i32, i32, i32, i32, i32, i32, i32, i32 }

define void @downsample_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 36 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 3
  %7 = icmp ult i32 %3, %6
  br i1 %7, label %8, label %155

; <label>:8                                       ; preds = %0
  %9 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.i32 %9, 0
  %11 = icmp ult i32 %4, %10
  br i1 %11, label %12, label %155

; <label>:12                                      ; preds = %8
  %13 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %14 = extractvalue %dx.types.CBufRet.i32 %13, 1
  %15 = extractvalue %dx.types.CBufRet.i32 %13, 2
  %16 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %17 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %18 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %14, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %19 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %18, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %20 = add i32 %17, %16
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %20, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 4098, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<4xF32>
  %23 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %24 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %23, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %25 = uitofp i32 %3 to float
  %26 = uitofp i32 %4 to float
  %27 = fadd fast float %25, 5.000000e-01
  %28 = fadd fast float %26, 5.000000e-01
  %29 = uitofp i32 %6 to float
  %30 = uitofp i32 %10 to float
  %31 = fdiv fast float 1.000000e+00, %29
  %32 = fdiv fast float 1.000000e+00, %30
  %33 = fmul fast float %31, %27
  %34 = fmul fast float %32, %28
  %35 = uitofp i32 %15 to float
  %36 = fsub fast float %33, %31
  %37 = fsub fast float %34, %32
  %38 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %36, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %39 = extractvalue %dx.types.ResRet.f32 %38, 0
  %40 = extractvalue %dx.types.ResRet.f32 %38, 1
  %41 = extractvalue %dx.types.ResRet.f32 %38, 2
  %42 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %33, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %43 = extractvalue %dx.types.ResRet.f32 %42, 0
  %44 = extractvalue %dx.types.ResRet.f32 %42, 1
  %45 = extractvalue %dx.types.ResRet.f32 %42, 2
  %46 = fadd fast float %33, %31
  %47 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %46, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %48 = extractvalue %dx.types.ResRet.f32 %47, 0
  %49 = extractvalue %dx.types.ResRet.f32 %47, 1
  %50 = extractvalue %dx.types.ResRet.f32 %47, 2
  %51 = fmul fast float %31, %25
  %52 = fmul fast float %32, %26
  %53 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %51, float %52, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %54 = extractvalue %dx.types.ResRet.f32 %53, 0
  %55 = extractvalue %dx.types.ResRet.f32 %53, 1
  %56 = extractvalue %dx.types.ResRet.f32 %53, 2
  %57 = fadd fast float %25, 1.000000e+00
  %58 = fmul fast float %31, %57
  %59 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %58, float %52, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %60 = extractvalue %dx.types.ResRet.f32 %59, 0
  %61 = extractvalue %dx.types.ResRet.f32 %59, 1
  %62 = extractvalue %dx.types.ResRet.f32 %59, 2
  %63 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %36, float %34, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %64 = extractvalue %dx.types.ResRet.f32 %63, 0
  %65 = extractvalue %dx.types.ResRet.f32 %63, 1
  %66 = extractvalue %dx.types.ResRet.f32 %63, 2
  %67 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %33, float %34, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %68 = extractvalue %dx.types.ResRet.f32 %67, 0
  %69 = extractvalue %dx.types.ResRet.f32 %67, 1
  %70 = extractvalue %dx.types.ResRet.f32 %67, 2
  %71 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %46, float %34, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %72 = extractvalue %dx.types.ResRet.f32 %71, 0
  %73 = extractvalue %dx.types.ResRet.f32 %71, 1
  %74 = extractvalue %dx.types.ResRet.f32 %71, 2
  %75 = fadd fast float %26, 1.000000e+00
  %76 = fmul fast float %32, %75
  %77 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %51, float %76, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %78 = extractvalue %dx.types.ResRet.f32 %77, 0
  %79 = extractvalue %dx.types.ResRet.f32 %77, 1
  %80 = extractvalue %dx.types.ResRet.f32 %77, 2
  %81 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %58, float %76, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %82 = extractvalue %dx.types.ResRet.f32 %81, 0
  %83 = extractvalue %dx.types.ResRet.f32 %81, 1
  %84 = extractvalue %dx.types.ResRet.f32 %81, 2
  %85 = fadd fast float %34, %32
  %86 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %36, float %85, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %87 = extractvalue %dx.types.ResRet.f32 %86, 0
  %88 = extractvalue %dx.types.ResRet.f32 %86, 1
  %89 = extractvalue %dx.types.ResRet.f32 %86, 2
  %90 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %33, float %85, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %91 = extractvalue %dx.types.ResRet.f32 %90, 0
  %92 = extractvalue %dx.types.ResRet.f32 %90, 1
  %93 = extractvalue %dx.types.ResRet.f32 %90, 2
  %94 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %19, %dx.types.Handle %24, float %46, float %85, float undef, float undef, i32 0, i32 0, i32 undef, float %35)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %95 = extractvalue %dx.types.ResRet.f32 %94, 0
  %96 = extractvalue %dx.types.ResRet.f32 %94, 1
  %97 = extractvalue %dx.types.ResRet.f32 %94, 2
  %98 = fadd fast float %60, %54
  %99 = fadd fast float %61, %55
  %100 = fadd fast float %62, %56
  %101 = fadd fast float %98, %78
  %102 = fadd fast float %99, %79
  %103 = fadd fast float %100, %80
  %104 = fadd fast float %101, %82
  %105 = fadd fast float %102, %83
  %106 = fadd fast float %103, %84
  %107 = fmul fast float %104, 1.250000e-01
  %108 = fmul fast float %105, 1.250000e-01
  %109 = fmul fast float %106, 1.250000e-01
  %110 = fmul fast float %91, 2.000000e+00
  %111 = fmul fast float %72, 2.000000e+00
  %112 = fmul fast float %68, 2.000000e+00
  %113 = fadd fast float %64, %43
  %114 = fadd fast float %113, %68
  %115 = fmul fast float %114, 2.000000e+00
  %116 = fadd fast float %48, %39
  %117 = fadd fast float %116, %87
  %118 = fadd fast float %117, %95
  %119 = fadd fast float %118, %110
  %120 = fadd fast float %119, %111
  %121 = fadd fast float %120, %112
  %122 = fadd fast float %121, %115
  %123 = fmul fast float %122, 3.125000e-02
  %124 = fadd fast float %123, %107
  %125 = fmul fast float %92, 2.000000e+00
  %126 = fmul fast float %73, 2.000000e+00
  %127 = fmul fast float %69, 2.000000e+00
  %128 = fadd fast float %65, %44
  %129 = fadd fast float %128, %69
  %130 = fmul fast float %129, 2.000000e+00
  %131 = fadd fast float %49, %40
  %132 = fadd fast float %131, %88
  %133 = fadd fast float %132, %96
  %134 = fadd fast float %133, %125
  %135 = fadd fast float %134, %126
  %136 = fadd fast float %135, %127
  %137 = fadd fast float %136, %130
  %138 = fmul fast float %137, 3.125000e-02
  %139 = fadd fast float %138, %108
  %140 = fmul fast float %93, 2.000000e+00
  %141 = fmul fast float %74, 2.000000e+00
  %142 = fmul fast float %70, 2.000000e+00
  %143 = fadd fast float %66, %45
  %144 = fadd fast float %143, %70
  %145 = fmul fast float %144, 2.000000e+00
  %146 = fadd fast float %50, %41
  %147 = fadd fast float %146, %89
  %148 = fadd fast float %147, %97
  %149 = fadd fast float %148, %140
  %150 = fadd fast float %149, %141
  %151 = fadd fast float %150, %142
  %152 = fadd fast float %151, %145
  %153 = fmul fast float %152, 3.125000e-02
  %154 = fadd fast float %153, %109
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %22, i32 %3, i32 %4, i32 undef, float %124, float %139, float %154, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %155

; <label>:155                                     ; preds = %12, %8, %0
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_downsample_bindings* undef, !"", i32 0, i32 0, i32 1, i32 36, null}
!6 = !{void ()* @downsample_cs, !"downsample_cs", null, !3, !7}
!7 = !{i32 0, i64 11819548928, i32 4, !8}
!8 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::saber_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(204812, 4804);
}

// shader_hash: b3eed3d3b1f779262a878da696bb3462
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 121
// dynamic_flow_control_count: 2
// float_instruction_count: 51
// int_instruction_count: 3
// texture_normal_instructions: 10
// texture_store_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; no parameters
; shader debug name: b3eed3d3b1f779262a878da696bb3462.pdb
; shader hash: b3eed3d3b1f779262a878da696bb3462
;
; Pipeline Runtime Information: 
;
; Compute Shader
; NumThreads=(8,8,1)
;
;
; Buffer Definitions:
;
; cbuffer g_upsample_bindings
; {
;
;   struct g_upsample_bindings
;   {
;
;       struct struct.UpsampleBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint src_dn_texture;                      ; Offset:    4
;           uint src_up_texture;                      ; Offset:    8
;           uint src_texture_level;                   ; Offset:   12
;           uint src_texture_width;                   ; Offset:   16
;           uint src_texture_height;                  ; Offset:   20
;           uint dst_texture;                         ; Offset:   24
;           uint dst_texture_level;                   ; Offset:   28
;           uint dst_texture_width;                   ; Offset:   32
;           uint dst_texture_height;                  ; Offset:   36
;       
;       } g_upsample_bindings;                        ; Offset:    0
;
;   
;   } g_upsample_bindings;                            ; Offset:    0 Size:    40
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_upsample_bindings               cbuffer      NA          NA     CB0            cb0     1
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_upsample_bindings = type { %struct.UpsampleBindings }
%struct.UpsampleBindings = type { i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 }

define void @upsample_cs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 40 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.threadId.i32(i32 93, i32 0)  ; ThreadId(component)
  %4 = call i32 @dx.op.threadId.i32(i32 93, i32 1)  ; ThreadId(component)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = icmp ult i32 %3, %6
  br i1 %7, label %8, label %119

; <label>:8                                       ; preds = %0
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %10 = icmp ult i32 %4, %9
  br i1 %10, label %11, label %119

; <label>:11                                      ; preds = %8
  %12 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %13 = extractvalue %dx.types.CBufRet.i32 %12, 1
  %14 = extractvalue %dx.types.CBufRet.i32 %12, 2
  %15 = extractvalue %dx.types.CBufRet.i32 %12, 3
  %16 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %17 = extractvalue %dx.types.CBufRet.i32 %16, 2
  %18 = extractvalue %dx.types.CBufRet.i32 %16, 3
  %19 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %13, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %20 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %19, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %21 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %14, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %22 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %21, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %23 = add i32 %18, %17
  %24 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %23, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %25 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %24, %dx.types.ResourceProperties { i32 4098, i32 1033 })  ; AnnotateHandle(res,props)  resource: RWTexture2D<4xF32>
  %26 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %27 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %26, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %28 = uitofp i32 %3 to float
  %29 = uitofp i32 %4 to float
  %30 = fadd fast float %28, 5.000000e-01
  %31 = fadd fast float %29, 5.000000e-01
  %32 = uitofp i32 %6 to float
  %33 = uitofp i32 %9 to float
  %34 = fdiv fast float 1.000000e+00, %32
  %35 = fdiv fast float 1.000000e+00, %33
  %36 = fmul fast float %34, %30
  %37 = fmul fast float %35, %31
  %38 = uitofp i32 %15 to float
  %39 = fsub fast float %36, %34
  %40 = fsub fast float %37, %35
  %41 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %39, float %40, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %42 = extractvalue %dx.types.ResRet.f32 %41, 0
  %43 = extractvalue %dx.types.ResRet.f32 %41, 1
  %44 = extractvalue %dx.types.ResRet.f32 %41, 2
  %45 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %36, float %40, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %46 = extractvalue %dx.types.ResRet.f32 %45, 0
  %47 = extractvalue %dx.types.ResRet.f32 %45, 1
  %48 = extractvalue %dx.types.ResRet.f32 %45, 2
  %49 = fadd fast float %36, %34
  %50 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %49, float %40, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %51 = extractvalue %dx.types.ResRet.f32 %50, 0
  %52 = extractvalue %dx.types.ResRet.f32 %50, 1
  %53 = extractvalue %dx.types.ResRet.f32 %50, 2
  %54 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %39, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %55 = extractvalue %dx.types.ResRet.f32 %54, 0
  %56 = extractvalue %dx.types.ResRet.f32 %54, 1
  %57 = extractvalue %dx.types.ResRet.f32 %54, 2
  %58 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %36, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %59 = extractvalue %dx.types.ResRet.f32 %58, 0
  %60 = extractvalue %dx.types.ResRet.f32 %58, 1
  %61 = extractvalue %dx.types.ResRet.f32 %58, 2
  %62 = fmul fast float %59, 4.000000e+00
  %63 = fmul fast float %60, 4.000000e+00
  %64 = fmul fast float %61, 4.000000e+00
  %65 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %49, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %66 = extractvalue %dx.types.ResRet.f32 %65, 0
  %67 = extractvalue %dx.types.ResRet.f32 %65, 1
  %68 = extractvalue %dx.types.ResRet.f32 %65, 2
  %69 = fadd fast float %37, %35
  %70 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %39, float %69, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %71 = extractvalue %dx.types.ResRet.f32 %70, 0
  %72 = extractvalue %dx.types.ResRet.f32 %70, 1
  %73 = extractvalue %dx.types.ResRet.f32 %70, 2
  %74 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %36, float %69, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %75 = extractvalue %dx.types.ResRet.f32 %74, 0
  %76 = extractvalue %dx.types.ResRet.f32 %74, 1
  %77 = extractvalue %dx.types.ResRet.f32 %74, 2
  %78 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %20, %dx.types.Handle %27, float %49, float %69, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %79 = extractvalue %dx.types.ResRet.f32 %78, 0
  %80 = extractvalue %dx.types.ResRet.f32 %78, 1
  %81 = extractvalue %dx.types.ResRet.f32 %78, 2
  %82 = call %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32 62, %dx.types.Handle %22, %dx.types.Handle %27, float %36, float %37, float undef, float undef, i32 0, i32 0, i32 undef, float %38)  ; SampleLevel(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,LOD)
  %83 = extractvalue %dx.types.ResRet.f32 %82, 0
  %84 = extractvalue %dx.types.ResRet.f32 %82, 1
  %85 = extractvalue %dx.types.ResRet.f32 %82, 2
  %86 = fadd fast float %55, %46
  %87 = fadd fast float %86, %66
  %88 = fadd fast float %87, %75
  %89 = fmul fast float %88, 2.000000e+00
  %90 = fadd fast float %51, %42
  %91 = fadd fast float %90, %62
  %92 = fadd fast float %91, %71
  %93 = fadd fast float %92, %79
  %94 = fadd fast float %93, %83
  %95 = fadd fast float %94, %89
  %96 = fadd fast float %56, %47
  %97 = fadd fast float %96, %67
  %98 = fadd fast float %97, %76
  %99 = fmul fast float %98, 2.000000e+00
  %100 = fadd fast float %52, %43
  %101 = fadd fast float %100, %63
  %102 = fadd fast float %101, %72
  %103 = fadd fast float %102, %80
  %104 = fadd fast float %103, %84
  %105 = fadd fast float %104, %99
  %106 = fadd fast float %57, %48
  %107 = fadd fast float %106, %68
  %108 = fadd fast float %107, %77
  %109 = fmul fast float %108, 2.000000e+00
  %110 = fadd fast float %53, %44
  %111 = fadd fast float %110, %64
  %112 = fadd fast float %111, %73
  %113 = fadd fast float %112, %81
  %114 = fadd fast float %113, %85
  %115 = fadd fast float %114, %109
  %116 = fmul fast float %95, 5.000000e-01
  %117 = fmul fast float %105, 5.000000e-01
  %118 = fmul fast float %115, 5.000000e-01
  call void @dx.op.textureStore.f32(i32 67, %dx.types.Handle %25, i32 %3, i32 %4, i32 undef, float %116, float %117, float %118, float 1.000000e+00, i8 15)  ; TextureStore(srv,coord0,coord1,coord2,value0,value1,value2,value3,mask)
  br label %119

; <label>:119                                     ; preds = %11, %8, %0
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.threadId.i32(i32, i32) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sampleLevel.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #1

; Function Attrs: nounwind
declare void @dx.op.textureStore.f32(i32, %dx.types.Handle, i32, i32, i32, float, float, float, float, i8) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind readonly }
attributes #2 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.entryPoints = !{!6}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"cs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_upsample_bindings* undef, !"", i32 0, i32 0, i32 1, i32 40, null}
!6 = !{void ()* @upsample_cs, !"upsample_cs", null, !3, !7}
!7 = !{i32 0, i64 11819548928, i32 4, !8}
!8 = !{i32 8, i32 8, i32 1}
*/
auto Shaders::saber_upsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(209616, 4628);
}

// shader_hash: d8a565ad99bfe7bd2ffa3d4dd9ecd7ad
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
/* disassembly:
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
; shader debug name: d8a565ad99bfe7bd2ffa3d4dd9ecd7ad.pdb
; shader hash: d8a565ad99bfe7bd2ffa3d4dd9ecd7ad
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Buffer Definitions:
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 6
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @blit_vs() {
  %1 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %2 = and i32 %1, 2
  %3 = uitofp i32 %2 to float
  %4 = shl i32 %1, 1
  %5 = and i32 %4, 2
  %6 = uitofp i32 %5 to float
  %7 = fmul fast float %3, 2.000000e+00
  %8 = fmul fast float %6, 2.000000e+00
  %9 = fadd fast float %7, -1.000000e+00
  %10 = fsub fast float 1.000000e+00, %8
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %9)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %10)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float 0.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %6)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.viewIdState = !{!3}
!dx.entryPoints = !{!4}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{[7 x i32] [i32 5, i32 6, i32 51, i32 0, i32 0, i32 0, i32 0]}
!4 = !{void ()* @blit_vs, !"blit_vs", !5, null, !16}
!5 = !{!6, !11, null}
!6 = !{!7, !10}
!7 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !8, i8 0, i32 1, i8 1, i32 0, i8 0, !9}
!8 = !{i32 0}
!9 = !{i32 3, i32 1}
!10 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !8, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!11 = !{!12, !14}
!12 = !{i32 0, !"SV_Position", i8 9, i8 3, !8, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !8, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 0, i64 8388864}
*/
auto Shaders::saber_blit_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(214244, 3268);
}

// shader_hash: e097ba37e35cd44359398b78600f7605
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 29
// float_instruction_count: 3
// texture_normal_instructions: 2
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xy          1     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: e097ba37e35cd44359398b78600f7605.pdb
; shader hash: e097ba37e35cd44359398b78600f7605
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_blit_bindings
; {
;
;   struct g_blit_bindings
;   {
;
;       struct struct.BlitBindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint scene_texture;                       ; Offset:    4
;           uint bloom_texture;                       ; Offset:    8
;       
;       } g_blit_bindings;                            ; Offset:    0
;
;   
;   } g_blit_bindings;                                ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_blit_bindings                   cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 6, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 4, 5 }
;   output 1 depends on inputs: { 4, 5 }
;   output 2 depends on inputs: { 4, 5 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_blit_bindings = type { %struct.BlitBindings }
%struct.BlitBindings = type { i32, i32, i32 }

define void @blit_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 1
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %9 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %10 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %9, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %11 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %10, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %14 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %8, %dx.types.Handle %13, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %15 = extractvalue %dx.types.ResRet.f32 %14, 0
  %16 = extractvalue %dx.types.ResRet.f32 %14, 1
  %17 = extractvalue %dx.types.ResRet.f32 %14, 2
  %18 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %11, %dx.types.Handle %13, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %19 = extractvalue %dx.types.ResRet.f32 %18, 0
  %20 = extractvalue %dx.types.ResRet.f32 %18, 1
  %21 = extractvalue %dx.types.ResRet.f32 %18, 2
  %22 = fadd fast float %19, %15
  %23 = fadd fast float %20, %16
  %24 = fadd fast float %21, %17
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %22)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %23)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %24)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float 1.000000e+00)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_blit_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[8 x i32] [i32 6, i32 4, i32 0, i32 0, i32 0, i32 0, i32 7, i32 7]}
!7 = !{void ()* @blit_ps, !"blit_ps", !8, !3, !17}
!8 = !{!9, !14, null}
!9 = !{!10, !12}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!13 = !{i32 3, i32 3}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 0, i64 3229614336}
*/
auto Shaders::saber_blit_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(217512, 4292);
}

// shader_hash: f811e0f46dd8880c55e845f1b43de01e
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 67
// float_instruction_count: 8
// texture_load_instructions: 3
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_VertexID              0   x           0   VERTID    uint   x   
; SV_InstanceID            0   x           1   INSTID    uint       
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; ATTRIBUTE                0   xyz         1     NONE   float   xyz 
; ATTRIBUTE                1   xy          2     NONE   float   xy  
;
; shader debug name: f811e0f46dd8880c55e845f1b43de01e.pdb
; shader hash: f811e0f46dd8880c55e845f1b43de01e
;
; Pipeline Runtime Information: 
;
; Vertex Shader
; OutputPositionPresent=1
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_VertexID              0                              
; SV_InstanceID            0                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 5, outputs: 10
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0 }
;   output 1 depends on inputs: { 0 }
;   output 2 depends on inputs: { 0 }
;   output 3 depends on inputs: { 0 }
;   output 4 depends on inputs: { 0 }
;   output 5 depends on inputs: { 0 }
;   output 6 depends on inputs: { 0 }
;   output 8 depends on inputs: { 0 }
;   output 9 depends on inputs: { 0 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @draw_vs() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call i32 @dx.op.loadInput.i32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %5 = extractvalue %dx.types.CBufRet.i32 %4, 0
  %6 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %5, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %7 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %6, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %8 = extractvalue %dx.types.CBufRet.i32 %4, 1
  %9 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %8, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %10 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %9, %dx.types.ResourceProperties { i32 524, i32 48 })  ; AnnotateHandle(res,props)  resource: StructuredBuffer<stride=48>
  %11 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 0, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %12 = extractvalue %dx.types.ResRet.f32 %11, 0
  %13 = extractvalue %dx.types.ResRet.f32 %11, 1
  %14 = extractvalue %dx.types.ResRet.f32 %11, 2
  %15 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 12, i8 7, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %16 = extractvalue %dx.types.ResRet.f32 %15, 0
  %17 = extractvalue %dx.types.ResRet.f32 %15, 1
  %18 = extractvalue %dx.types.ResRet.f32 %15, 2
  %19 = call %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32 139, %dx.types.Handle %10, i32 %3, i32 24, i8 3, i32 4)  ; RawBufferLoad(srv,index,elementOffset,mask,alignment)
  %20 = extractvalue %dx.types.ResRet.f32 %19, 0
  %21 = extractvalue %dx.types.ResRet.f32 %19, 1
  %22 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %23 = extractvalue %dx.types.CBufRet.f32 %22, 0
  %24 = extractvalue %dx.types.CBufRet.f32 %22, 1
  %25 = extractvalue %dx.types.CBufRet.f32 %22, 2
  %26 = extractvalue %dx.types.CBufRet.f32 %22, 3
  %27 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %28 = extractvalue %dx.types.CBufRet.f32 %27, 0
  %29 = extractvalue %dx.types.CBufRet.f32 %27, 1
  %30 = extractvalue %dx.types.CBufRet.f32 %27, 2
  %31 = extractvalue %dx.types.CBufRet.f32 %27, 3
  %32 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 2)  ; CBufferLoadLegacy(handle,regIndex)
  %33 = extractvalue %dx.types.CBufRet.f32 %32, 0
  %34 = extractvalue %dx.types.CBufRet.f32 %32, 1
  %35 = extractvalue %dx.types.CBufRet.f32 %32, 2
  %36 = extractvalue %dx.types.CBufRet.f32 %32, 3
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %7, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = extractvalue %dx.types.CBufRet.f32 %37, 1
  %40 = extractvalue %dx.types.CBufRet.f32 %37, 2
  %41 = extractvalue %dx.types.CBufRet.f32 %37, 3
  %42 = fmul fast float %23, %12
  %43 = call float @dx.op.tertiary.f32(i32 46, float %28, float %13, float %42)  ; FMad(a,b,c)
  %44 = call float @dx.op.tertiary.f32(i32 46, float %33, float %14, float %43)  ; FMad(a,b,c)
  %45 = fadd fast float %44, %38
  %46 = fmul fast float %24, %12
  %47 = call float @dx.op.tertiary.f32(i32 46, float %29, float %13, float %46)  ; FMad(a,b,c)
  %48 = call float @dx.op.tertiary.f32(i32 46, float %34, float %14, float %47)  ; FMad(a,b,c)
  %49 = fadd fast float %48, %39
  %50 = fmul fast float %25, %12
  %51 = call float @dx.op.tertiary.f32(i32 46, float %30, float %13, float %50)  ; FMad(a,b,c)
  %52 = call float @dx.op.tertiary.f32(i32 46, float %35, float %14, float %51)  ; FMad(a,b,c)
  %53 = fadd fast float %52, %40
  %54 = fmul fast float %26, %12
  %55 = call float @dx.op.tertiary.f32(i32 46, float %31, float %13, float %54)  ; FMad(a,b,c)
  %56 = call float @dx.op.tertiary.f32(i32 46, float %36, float %14, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %41
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %45)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %49)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %53)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %16)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 2, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %21)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare i32 @dx.op.loadInput.i32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.rawBufferLoad.f32(i32, %dx.types.Handle, i32, i32, i8, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"vs", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[7 x i32] [i32 5, i32 10, i32 895, i32 0, i32 0, i32 0, i32 0]}
!7 = !{void ()* @draw_vs, !"draw_vs", !8, !3, !22}
!8 = !{!9, !14, null}
!9 = !{!10, !13}
!10 = !{i32 0, !"SV_VertexID", i8 5, i8 1, !11, i8 0, i32 1, i8 1, i32 0, i8 0, !12}
!11 = !{i32 0}
!12 = !{i32 3, i32 1}
!13 = !{i32 1, !"SV_InstanceID", i8 5, i8 2, !11, i8 0, i32 1, i8 1, i32 1, i8 0, null}
!14 = !{!15, !17, !19}
!15 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, !16}
!16 = !{i32 3, i32 15}
!17 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, !18}
!18 = !{i32 3, i32 7}
!19 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !20, i8 2, i32 1, i8 2, i32 2, i8 0, !21}
!20 = !{i32 1}
!21 = !{i32 3, i32 3}
!22 = !{i32 0, i64 1082130688}
*/
auto Shaders::grass_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(221804, 5036);
}

// shader_hash: 528770a44c3f3470b8bad92fc30022a2
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 28
// float_instruction_count: 2
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 528770a44c3f3470b8bad92fc30022a2.pdb
; shader hash: 528770a44c3f3470b8bad92fc30022a2
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 10, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 8, 9 }
;   output 1 depends on inputs: { 8, 9 }
;   output 2 depends on inputs: { 8, 9 }
;   output 3 depends on inputs: { 8, 9 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @draw_naive_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.f32 %9, 0
  %11 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %16 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %13, %dx.types.Handle %15, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %17 = extractvalue %dx.types.ResRet.f32 %16, 0
  %18 = extractvalue %dx.types.ResRet.f32 %16, 1
  %19 = extractvalue %dx.types.ResRet.f32 %16, 2
  %20 = extractvalue %dx.types.ResRet.f32 %16, 3
  %21 = fsub fast float %20, %10
  %22 = fcmp fast olt float %21, 0.000000e+00
  call void @dx.op.discard(i32 82, i1 %22)  ; Discard(condition)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %19)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %20)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind
declare void @dx.op.discard(i32, i1) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[12 x i32] [i32 10, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!7 = !{void ()* @draw_naive_ps, !"draw_naive_ps", !8, !3, !19}
!8 = !{!9, !16, null}
!9 = !{!10, !12, !13}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!13 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !14, i8 2, i32 1, i8 2, i32 2, i8 0, !15}
!14 = !{i32 1}
!15 = !{i32 3, i32 3}
!16 = !{!17}
!17 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !18}
!18 = !{i32 3, i32 15}
!19 = !{i32 0, i64 3229614336}
*/
auto Shaders::grass_draw_naive_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(226840, 4620);
}

// shader_hash: 4afc5d1154d78107f4f7353c14a71966
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 34
// float_instruction_count: 7
// texture_normal_instructions: 1
/* disassembly:
;
; Note: shader requires additional functionality:
;       Resource descriptor heap indexing
;       Sampler descriptor heap indexing
;
;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float       
; ATTRIBUTE                0   xyz         1     NONE   float       
; ATTRIBUTE                1   xy          2     NONE   float   xy  
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 4afc5d1154d78107f4f7353c14a71966.pdb
; shader hash: 4afc5d1154d78107f4f7353c14a71966
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; ATTRIBUTE                0                 linear       
; ATTRIBUTE                1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer g_bindings
; {
;
;   struct g_bindings
;   {
;
;       struct struct.Bindings
;       {
;
;           uint constants;                           ; Offset:    0
;           uint vertices;                            ; Offset:    4
;           uint texture;                             ; Offset:    8
;       
;       } g_bindings;                                 ; Offset:    0
;
;   
;   } g_bindings;                                     ; Offset:    0 Size:    12
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; g_bindings                        cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 10, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 8, 9 }
;   output 1 depends on inputs: { 8, 9 }
;   output 2 depends on inputs: { 8, 9 }
;   output 3 depends on inputs: { 8, 9 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:8-i16:16-i32:32-i64:64-f16:16-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.ResBind = type { i32, i32, i32, i8 }
%dx.types.ResourceProperties = type { i32, i32 }
%dx.types.CBufRet.i32 = type { i32, i32, i32, i32 }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.types.ResRet.f32 = type { float, float, float, float, i32 }
%g_bindings = type { %struct.Bindings }
%struct.Bindings = type { i32, i32, i32 }

define void @draw_atoc_ps() {
  %1 = call %dx.types.Handle @dx.op.createHandleFromBinding(i32 217, %dx.types.ResBind { i32 0, i32 0, i32 0, i8 2 }, i32 0, i1 false)  ; CreateHandleFromBinding(bind,index,nonUniformIndex)
  %2 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %1, %dx.types.ResourceProperties { i32 13, i32 12 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32 59, %dx.types.Handle %2, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %6 = extractvalue %dx.types.CBufRet.i32 %5, 0
  %7 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %6, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %8 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %7, %dx.types.ResourceProperties { i32 13, i32 820 })  ; AnnotateHandle(res,props)  resource: CBuffer
  %9 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %8, i32 4)  ; CBufferLoadLegacy(handle,regIndex)
  %10 = extractvalue %dx.types.CBufRet.f32 %9, 0
  %11 = extractvalue %dx.types.CBufRet.i32 %5, 2
  %12 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 %11, i1 false, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %13 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %12, %dx.types.ResourceProperties { i32 2, i32 1033 })  ; AnnotateHandle(res,props)  resource: Texture2D<4xF32>
  %14 = call %dx.types.Handle @dx.op.createHandleFromHeap(i32 218, i32 0, i1 true, i1 false)  ; CreateHandleFromHeap(index,samplerHeap,nonUniformIndex)
  %15 = call %dx.types.Handle @dx.op.annotateHandle(i32 216, %dx.types.Handle %14, %dx.types.ResourceProperties { i32 14, i32 0 })  ; AnnotateHandle(res,props)  resource: SamplerState
  %16 = call %dx.types.ResRet.f32 @dx.op.sample.f32(i32 60, %dx.types.Handle %13, %dx.types.Handle %15, float %3, float %4, float undef, float undef, i32 0, i32 0, i32 undef, float undef)  ; Sample(srv,sampler,coord0,coord1,coord2,coord3,offset0,offset1,offset2,clamp)
  %17 = extractvalue %dx.types.ResRet.f32 %16, 0
  %18 = extractvalue %dx.types.ResRet.f32 %16, 1
  %19 = extractvalue %dx.types.ResRet.f32 %16, 2
  %20 = extractvalue %dx.types.ResRet.f32 %16, 3
  %21 = fsub fast float %20, %10
  %22 = call float @dx.op.unary.f32(i32 83, float %20)  ; DerivCoarseX(value)
  %23 = call float @dx.op.unary.f32(i32 6, float %22)  ; FAbs(value)
  %24 = call float @dx.op.unary.f32(i32 84, float %20)  ; DerivCoarseY(value)
  %25 = call float @dx.op.unary.f32(i32 6, float %24)  ; FAbs(value)
  %26 = fadd fast float %25, %23
  %27 = call float @dx.op.binary.f32(i32 35, float %26, float 0x3F20000000000000)  ; FMax(a,b)
  %28 = fdiv fast float %21, %27
  %29 = fadd fast float %28, 5.000000e-01
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %17)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %18)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %19)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %29)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromHeap(i32, i32, i1, i1) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readonly
declare %dx.types.ResRet.f32 @dx.op.sample.f32(i32, %dx.types.Handle, %dx.types.Handle, float, float, float, float, i32, i32, i32, float) #2

; Function Attrs: nounwind readnone
declare float @dx.op.binary.f32(i32, float, float) #0

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.i32 @dx.op.cbufferLoadLegacy.i32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.annotateHandle(i32, %dx.types.Handle, %dx.types.ResourceProperties) #0

; Function Attrs: nounwind readnone
declare %dx.types.Handle @dx.op.createHandleFromBinding(i32, %dx.types.ResBind, i32, i1) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!1}
!dx.shaderModel = !{!2}
!dx.resources = !{!3}
!dx.viewIdState = !{!6}
!dx.entryPoints = !{!7}

!0 = !{!"dxcoob 1.8.2405.15 (fd7e54bcd)"}
!1 = !{i32 1, i32 8}
!2 = !{!"ps", i32 6, i32 8}
!3 = !{null, null, !4, null}
!4 = !{!5}
!5 = !{i32 0, %g_bindings* undef, !"", i32 0, i32 0, i32 1, i32 12, null}
!6 = !{[12 x i32] [i32 10, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 15, i32 15]}
!7 = !{void ()* @draw_atoc_ps, !"draw_atoc_ps", !8, !3, !19}
!8 = !{!9, !16, null}
!9 = !{!10, !12, !13}
!10 = !{i32 0, !"SV_Position", i8 9, i8 3, !11, i8 4, i32 1, i8 4, i32 0, i8 0, null}
!11 = !{i32 0}
!12 = !{i32 1, !"ATTRIBUTE", i8 9, i8 0, !11, i8 2, i32 1, i8 3, i32 1, i8 0, null}
!13 = !{i32 2, !"ATTRIBUTE", i8 9, i8 0, !14, i8 2, i32 1, i8 2, i32 2, i8 0, !15}
!14 = !{i32 1}
!15 = !{i32 3, i32 3}
!16 = !{!17}
!17 = !{i32 0, !"SV_Target", i8 9, i8 16, !11, i8 0, i32 1, i8 4, i32 0, i8 0, !18}
!18 = !{i32 3, i32 15}
!19 = !{i32 0, i64 3229614336}
*/
auto Shaders::grass_draw_atoc_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(231460, 4760);
}

#undef texture_data

} // namespace fb::baked::buffet
