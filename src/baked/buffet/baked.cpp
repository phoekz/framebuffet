#include "baked.hpp"

namespace fb::baked::buffet {

#define texture_data(rp, sp, off, sz)                                                   \
    TextureData {                                                                       \
        .row_pitch = rp, .slice_pitch = sp, .data = transmuted_span<std::byte>(off, sz) \
    }

Assets::Assets() {
    // hash: 3167c2e8b2bbb349fed818fbe7bc94d2
    _data = read_whole_file("fb_buffet_assets.bin");
    FB_ASSERT(_data.size() == 176272592);
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
    return Mesh {
        // hash: 576ddd3553b833a1778d067f0cfc0f20
        .vertices = transmuted_span<Vertex>(4088, 2025),
        // hash: b4d34f8bbc09c51115542a8b94aec26a
        .indices = transmuted_span<Index>(101288, 5301),
    };
}

auto Assets::sci_fi_case_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,    122492, 4194304); // hash: 1c0cad906e975f6b2b0f9d24de7c4926, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,   4316796, 1048576); // hash: 0fc5f72ca36e69f9ee7aa3e1007128da, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,   5365372,  262144); // hash: 5a31a9aea69b9572403af8c398ff1bb1, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,   5627516,   65536); // hash: a7885b770b2e31e7cc2b44a128f01c11, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,   5693052,   16384); // hash: 57315ba87386a5f4d7792ac48fafbcc2, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,   5709436,    4096); // hash: a58e3210388634d7155f2901e7501649, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,   5713532,    1024); // hash: 148fb23c5ce094c10a5185b87ded78f6, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,   5714556,     256); // hash: 2b29afab47c66cc6470d9fd805320d9c, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,   5714812,      64); // hash: 618df3e3abd8ce160684dd0e4549ce74, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,   5714876,      16); // hash: 774155eac0ec01f795a1255477bebbb4, width: 2, height: 2
    datas[10] = texture_data(   4,       4,   5714892,       4); // hash: f27ac865f3272414b83c54340f946f64, width: 1, height: 1
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
    datas[ 0] = texture_data(4096, 4194304,   5714896, 4194304); // hash: d5c691d45d7f61290eb8b12ed954e8f1, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,   9909200, 1048576); // hash: 8bd2e1f3f4407531cc8243c08e556004, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  10957776,  262144); // hash: 65ad91e067821102e44d6e3811da1611, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  11219920,   65536); // hash: 01920ca08158b9166b3b5f99ea99a101, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  11285456,   16384); // hash: ab34727e8f142779cfe5eedc131212a3, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  11301840,    4096); // hash: dfe0b70bf322d90fb2bff80c5ab1d8c9, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  11305936,    1024); // hash: b3aa202e184073b05965c82d4e60ec78, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  11306960,     256); // hash: a592dfd5855b6ee57b42d9edfa7f2833, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  11307216,      64); // hash: c3439bbdf4acc22b41baf1a96203e800, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  11307280,      16); // hash: 17e7ca5da0e3c69176067d4cf9dff17a, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  11307296,       4); // hash: f334417f8373d6f0ef2e2f2651e34647, width: 1, height: 1
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
    datas[ 0] = texture_data(4096, 4194304,  11307300, 4194304); // hash: ed04eb3dfe14725c0ad06ee90e903d7b, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  15501604, 1048576); // hash: bef9846f192c1a321592fe54f565ab64, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  16550180,  262144); // hash: 3114a59afa1940f13944b1903aecca77, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  16812324,   65536); // hash: af552342df2d95a4e8db91c262d030ad, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  16877860,   16384); // hash: ae6b271b6cedc3117259743503987475, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  16894244,    4096); // hash: 803f12a9da7dc4117b935a6570eb6e62, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  16898340,    1024); // hash: 2de9c42bfe1107d80ba68be92c4133dd, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  16899364,     256); // hash: d8a8fba03904f3e9c7123cb6b038fece, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  16899620,      64); // hash: 5361feba23fce4703e3c634048c080d7, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  16899684,      16); // hash: f2b6d357693a6c96c8686a98bd80974b, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  16899700,       4); // hash: a67c136df57f927e6e07eab72b4f022b, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::metal_plane_mesh() const -> Mesh {
    // vertex_count: 4
    // face_count: 2
    return Mesh {
        // hash: 7513db2c165692b22a3bb4374738dab7
        .vertices = transmuted_span<Vertex>(16899704, 4),
        // hash: 96880b352a2cd08ebe7a559eac217606
        .indices = transmuted_span<Index>(16899896, 6),
    };
}

auto Assets::metal_plane_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(4096, 4194304,  16899920, 4194304); // hash: b01e0981487f9386f14d7e555623c2ba, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  21094224, 1048576); // hash: 75c211006262e9722c80d57b27242a91, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  22142800,  262144); // hash: 000fa85c2ab807e76e30458d71683c52, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  22404944,   65536); // hash: 5785c050ddf763147be0e8a10f2fd28b, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  22470480,   16384); // hash: 257a6272975283c2ba3982e0f48f3871, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  22486864,    4096); // hash: f3e2a564cd47f149462ab82d010ac6fc, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  22490960,    1024); // hash: 043e70e3d266bc8fa1af4ad7203787c9, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  22491984,     256); // hash: 144abce73f4bb6a7748614fffa83c5cd, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  22492240,      64); // hash: de1352b64f46a4757900083a9975b5a4, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  22492304,      16); // hash: 6d587cac4b377cdce6b029d7b6aa9ed6, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  22492320,       4); // hash: 275847d06c6d2099829491e6f3f23a61, width: 1, height: 1
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
    datas[ 0] = texture_data(4096, 4194304,  22492324, 4194304); // hash: 2c3220bc625827e8a3a07df006fd2577, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  26686628, 1048576); // hash: fc8ccb58611836d0fe177ea6bb751f8c, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  27735204,  262144); // hash: c79ba2ff35a7bea09af7cedce7153cb6, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  27997348,   65536); // hash: 861b5f55bc01af94eccb54fadede533a, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  28062884,   16384); // hash: c6241fb22bbdb204055a36c59e19eaa7, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  28079268,    4096); // hash: 96a28968bbeb40fd031cca35a218225c, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  28083364,    1024); // hash: c588a82ede6113e9a066f3ae10347b68, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  28084388,     256); // hash: 23f1fc279c052f32433e694e9dcf34d8, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  28084644,      64); // hash: cfc2177f15b55d6e9cd459a7ec5de27c, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  28084708,      16); // hash: 56c2be73c379af90ab87d34bc436619b, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  28084724,       4); // hash: 01862aadb9ce1ab37fe3803a108ced53, width: 1, height: 1
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
    datas[ 0] = texture_data(4096, 4194304,  28084728, 4194304); // hash: a0a8987101c3763d88cded178f004edc, width: 1024, height: 1024
    datas[ 1] = texture_data(2048, 1048576,  32279032, 1048576); // hash: 703b054f217ed2a761781a5790cc9da1, width: 512, height: 512
    datas[ 2] = texture_data(1024,  262144,  33327608,  262144); // hash: 2fdd37d099fbc0a45280294c7e26bda2, width: 256, height: 256
    datas[ 3] = texture_data( 512,   65536,  33589752,   65536); // hash: 52a2e110668972c369b81ca2f7d03732, width: 128, height: 128
    datas[ 4] = texture_data( 256,   16384,  33655288,   16384); // hash: 8df31f4575f5e3b5bdeb96cc013a2505, width: 64, height: 64
    datas[ 5] = texture_data( 128,    4096,  33671672,    4096); // hash: f06fd168844ac517f257d6cde21e3c3e, width: 32, height: 32
    datas[ 6] = texture_data(  64,    1024,  33675768,    1024); // hash: fd4f859331efb591100afda7bd764091, width: 16, height: 16
    datas[ 7] = texture_data(  32,     256,  33676792,     256); // hash: e5c5824300a76bb01e0e389a992d2eb8, width: 8, height: 8
    datas[ 8] = texture_data(  16,      64,  33677048,      64); // hash: 0f88755d2cf041ee01b24e89928c89f4, width: 4, height: 4
    datas[ 9] = texture_data(   8,      16,  33677112,      16); // hash: f77b6b506c3f744884e3d267355042be, width: 2, height: 2
    datas[10] = texture_data(   4,       4,  33677128,       4); // hash: eb09f6f496892ae3a96abfd9da49d00b, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .mip_count = 11,
        .datas = datas,
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    // vertex_count: 725
    // face_count: 678
    return Mesh {
        // hash: cee14e1c21b8adac5f904691e307303a
        .vertices = transmuted_span<Vertex>(33677132, 725),
        // hash: 2e21503f47bdce31841a1d7890743c68
        .indices = transmuted_span<Index>(33711932, 2034),
    };
}

auto Assets::coconut_tree_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(2048, 1048576,  33720068, 1048576); // hash: 61b50bca3a05bbee4e3297d007f5fc25, width: 512, height: 512
    datas[ 1] = texture_data(1024,  262144,  34768644,  262144); // hash: 0edae200d9d108428bd9b503d8169fbf, width: 256, height: 256
    datas[ 2] = texture_data( 512,   65536,  35030788,   65536); // hash: 594493b0020d694430b571fc79867452, width: 128, height: 128
    datas[ 3] = texture_data( 256,   16384,  35096324,   16384); // hash: 7c277a922339cb6f8cee9b7d4fb0d456, width: 64, height: 64
    datas[ 4] = texture_data( 128,    4096,  35112708,    4096); // hash: 2251821b2955fa891380f4eac15bcd6c, width: 32, height: 32
    datas[ 5] = texture_data(  64,    1024,  35116804,    1024); // hash: 5b943a736dfa82072e5cb6affe649865, width: 16, height: 16
    datas[ 6] = texture_data(  32,     256,  35117828,     256); // hash: c842b2708e4811562e58fa88d52283be, width: 8, height: 8
    datas[ 7] = texture_data(  16,      64,  35118084,      64); // hash: 4b46d1e4178e56188b4c00c7fc23cb42, width: 4, height: 4
    datas[ 8] = texture_data(   8,      16,  35118148,      16); // hash: 570f797ebb674c8e914531a49491f97f, width: 2, height: 2
    datas[ 9] = texture_data(   4,       4,  35118164,       4); // hash: 68d5e1ca0194c1023749ac2880a275c9, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .mip_count = 10,
        .datas = datas,
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    // vertex_count: 4
    // face_count: 2
    return Mesh {
        // hash: 8c75d3a9d4a8e7e3a9a7e30729dcb460
        .vertices = transmuted_span<Vertex>(35118168, 4),
        // hash: 96880b352a2cd08ebe7a559eac217606
        .indices = transmuted_span<Index>(35118360, 6),
    };
}

auto Assets::sand_plane_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data( 256,   16384,  35118384,   16384); // hash: cd9c6abea5eb3cc9b5846ab373f9d5d8, width: 64, height: 64
    datas[ 1] = texture_data( 128,    4096,  35134768,    4096); // hash: da49ee29170ccee71bde3a122fb1ae75, width: 32, height: 32
    datas[ 2] = texture_data(  64,    1024,  35138864,    1024); // hash: 63e53ce8aff7a307f0c1097ccf5d80c5, width: 16, height: 16
    datas[ 3] = texture_data(  32,     256,  35139888,     256); // hash: 00f8f27693c1fe15fbdd5456945dfa9f, width: 8, height: 8
    datas[ 4] = texture_data(  16,      64,  35140144,      64); // hash: 5743429ba6b07a05fffee7cf591b7cc5, width: 4, height: 4
    datas[ 5] = texture_data(   8,      16,  35140208,      16); // hash: c32f5c05e144ea6a196dd524d8dae576, width: 2, height: 2
    datas[ 6] = texture_data(   4,       4,  35140224,       4); // hash: 8005525e757dc43ff5d1826186e49ea5, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .mip_count = 7,
        .datas = datas,
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        // hash: e65e9d279ecf4a2eac69f51658a72576
        .skinning_vertices = transmuted_span<SkinningVertex>(35140228, 2430),
        // hash: fd12995a38e89146d53c7edd4f6cd3d0
        .indices = transmuted_span<Index>(35334628, 3102),
        // hash: d730e286c5631c1fdfa6f0f7191ddd3b
        .joint_nodes = transmuted_span<uint>(35347036, 63),
        // hash: ee0ff01354a210fae3930c7992929c24
        .joint_inverse_binds = transmuted_span<float4x4>(35347288, 63),
        // hash: e95d90faa9a34f6172c2c7b88a91046c
        .node_parents = transmuted_span<uint>(35351320, 72),
        // hash: a3ce072a99fe8139419ff55378991f51
        .node_transforms = transmuted_span<float4x4>(35351608, 72),
        // hash: 2ba2f2a43abbd5968d77adad334f5682
        .node_channels = transmuted_span<AnimationChannel>(35356216, 72),
        // hash: 3b153cfb6ae1bff95aa6dce90c05c435
        .node_channels_times_t = transmuted_span<float>(35359672, 2256),
        // hash: c2293a9e0414fc79c6d7cd00d4c1ba87
        .node_channels_times_r = transmuted_span<float>(35368696, 2274),
        // hash: 4d32d6eb8afd6df010a01af960891484
        .node_channels_times_s = transmuted_span<float>(35377792, 369),
        // hash: 1defd6ee1077f6a108951a438abf4c61
        .node_channels_values_t = transmuted_span<float3>(35379268, 2256),
        // hash: f00545a5efeaef5e3e8a97a62d2ae87a
        .node_channels_values_r = transmuted_span<Quaternion>(35406340, 2274),
        // hash: f56ee25392a3504e5a600f0f847638e4
        .node_channels_values_s = transmuted_span<float3>(35442724, 369),
    };
}

auto Assets::raccoon_base_color_texture() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(  16,      64,  35447152,      64); // hash: f46411895f74b52626f359cce1525b8e, width: 4, height: 4
    datas[ 1] = texture_data(   8,      16,  35447216,      16); // hash: 937e68bc91d5794588f960b38a352b2d, width: 2, height: 2
    datas[ 2] = texture_data(   4,       4,  35447232,       4); // hash: 44cbb8d6aae19256febe1555bfc8a1a1, width: 1, height: 1
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
    datas[ 0] = texture_data(  16,      64,  35447236,      64); // hash: 22771420d888fb100e7450610145f45d, width: 4, height: 4
    datas[ 1] = texture_data(   8,      16,  35447300,      16); // hash: 0eea1d73e7ab0514131b39b1aad79493, width: 2, height: 2
    datas[ 2] = texture_data(   4,       4,  35447316,       4); // hash: 44ff0e0fb6c73dbbd0b61d69281ee072, width: 1, height: 1
    // clang-format on
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 4,
        .height = 4,
        .mip_count = 3,
        .datas = datas,
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    // vertex_count: 24
    // face_count: 12
    return Mesh {
        // hash: d83d3ba89f1591682879b254506b0400
        .vertices = transmuted_span<Vertex>(35447320, 24),
        // hash: 616e076015f03b0288fd27d32b7bf256
        .indices = transmuted_span<Index>(35448472, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    // vertex_count: 24
    // face_count: 12
    return Mesh {
        // hash: 952f25941581c525b7890e0284fd3b52
        .vertices = transmuted_span<Vertex>(35448616, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(35449768, 36),
    };
}

auto Assets::sphere_mesh() const -> Mesh {
    // vertex_count: 2145
    // face_count: 4160
    return Mesh {
        // hash: bdea0aeb8805424705ca957338103a57
        .vertices = transmuted_span<Vertex>(35449912, 2145),
        // hash: b69e987626eec7de0864668ffde28fbe
        .indices = transmuted_span<Index>(35552872, 12480),
    };
}

auto Assets::winter_evening_lut() const -> Texture {
    decltype(Texture::datas) datas = {};
    // clang-format off
    datas[ 0] = texture_data(2048, 1048576,  35602792, 1048576); // hash: 7f7048fcf260a2aa1eae75b62340d74d, width: 512, height: 512
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
    datas[0][ 0] = texture_data(1024,  131072,  36651368,  131072); // hash: f1fca1ec93aaac5c8692900b209366a4, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072,  36782440,  131072); // hash: ebdcd9687791a33db4101d416f5e246c, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072,  36913512,  131072); // hash: 9747b535e689e870395045429a78a958, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072,  37044584,  131072); // hash: a392c47563dd2d8f529d6016031ce565, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072,  37175656,  131072); // hash: e70d52519c582ded8307aecbc69261c5, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072,  37306728,  131072); // hash: 06d3e956fa5a867dfb0e6e272bfcc87f, width: 128, height: 128
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
    datas[0][ 0] = texture_data(8192, 8388608,  37437800, 8388608); // hash: e70fec4202154d74b817ce87548a491d, width: 1024, height: 1024
    datas[0][ 1] = texture_data(4096, 2097152,  45826408, 2097152); // hash: 85fc929ac43a8d3c7caa5685b297bde8, width: 512, height: 512
    datas[0][ 2] = texture_data(2048,  524288,  47923560,  524288); // hash: 271f1d9f9ae40adfd6ee286fd23d39f9, width: 256, height: 256
    datas[0][ 3] = texture_data(1024,  131072,  48447848,  131072); // hash: dd2419288db4af6d0254f8d7c6822db0, width: 128, height: 128
    datas[0][ 4] = texture_data( 512,   32768,  48578920,   32768); // hash: b7ce6c5e2b8a2132421a1c608e81ba76, width: 64, height: 64
    datas[0][ 5] = texture_data( 256,    8192,  48611688,    8192); // hash: 972ee92b49ddbd3d926c06d10509b884, width: 32, height: 32
    datas[0][ 6] = texture_data( 128,    2048,  48619880,    2048); // hash: d76513ed7aac113939ce51b47e4e2141, width: 16, height: 16
    datas[0][ 7] = texture_data(  64,     512,  48621928,     512); // hash: 7a7ae37458be6407af077e0109f9f2cc, width: 8, height: 8
    datas[0][ 8] = texture_data(  32,     128,  48622440,     128); // hash: 53b80a4f80cd4f35d4eac8a528355cdc, width: 4, height: 4
    datas[0][ 9] = texture_data(  16,      32,  48622568,      32); // hash: 6ea38dfc8963ee6c09ea67573b990ad0, width: 2, height: 2
    datas[0][10] = texture_data(   8,       8,  48622600,       8); // hash: b6faa2f429d0cf1ecf90387a19007e2b, width: 1, height: 1
    datas[1][ 0] = texture_data(8192, 8388608,  48622608, 8388608); // hash: dbf0ebc6a6c592cae9ad8e3a3f2568aa, width: 1024, height: 1024
    datas[1][ 1] = texture_data(4096, 2097152,  57011216, 2097152); // hash: c56c4e38c4b497e7d2896d0022f7e830, width: 512, height: 512
    datas[1][ 2] = texture_data(2048,  524288,  59108368,  524288); // hash: 284c5bf396722b2c924130675a6259d3, width: 256, height: 256
    datas[1][ 3] = texture_data(1024,  131072,  59632656,  131072); // hash: 7b46ba89a5ba62fce63a10e903333ba0, width: 128, height: 128
    datas[1][ 4] = texture_data( 512,   32768,  59763728,   32768); // hash: 8e136d5fc0581d72e060c750ab7fc58f, width: 64, height: 64
    datas[1][ 5] = texture_data( 256,    8192,  59796496,    8192); // hash: 086133860ad9c48e8524f70cb010ca4d, width: 32, height: 32
    datas[1][ 6] = texture_data( 128,    2048,  59804688,    2048); // hash: 9002e8a826bfee8cc8ef4e8a986c32c1, width: 16, height: 16
    datas[1][ 7] = texture_data(  64,     512,  59806736,     512); // hash: f16b5c6316e9a8e819591d2bfeeced15, width: 8, height: 8
    datas[1][ 8] = texture_data(  32,     128,  59807248,     128); // hash: fe94bf054fd0fa5914e85a3fd840fe83, width: 4, height: 4
    datas[1][ 9] = texture_data(  16,      32,  59807376,      32); // hash: 7c810dac33ceaf7725a8b6a3b4a6b761, width: 2, height: 2
    datas[1][10] = texture_data(   8,       8,  59807408,       8); // hash: 95ace211749b7db4373ffb99761b93a6, width: 1, height: 1
    datas[2][ 0] = texture_data(8192, 8388608,  59807416, 8388608); // hash: 60fc398a716fe918ea5179abdbed5da6, width: 1024, height: 1024
    datas[2][ 1] = texture_data(4096, 2097152,  68196024, 2097152); // hash: 6b56e3c11a6f4266150518a3b87f34c9, width: 512, height: 512
    datas[2][ 2] = texture_data(2048,  524288,  70293176,  524288); // hash: 4194387a266567bc073234a28488ab54, width: 256, height: 256
    datas[2][ 3] = texture_data(1024,  131072,  70817464,  131072); // hash: dda04d5b17e662698ce7d1f09929f5fb, width: 128, height: 128
    datas[2][ 4] = texture_data( 512,   32768,  70948536,   32768); // hash: e36f842c91fd700c1e324c56690d3d05, width: 64, height: 64
    datas[2][ 5] = texture_data( 256,    8192,  70981304,    8192); // hash: 5636f81a9c40ddbd37ad4c2be787db58, width: 32, height: 32
    datas[2][ 6] = texture_data( 128,    2048,  70989496,    2048); // hash: ad241b34b4124b18fb8da354e42cf306, width: 16, height: 16
    datas[2][ 7] = texture_data(  64,     512,  70991544,     512); // hash: 89495bdab42a8777f81c94e53033194c, width: 8, height: 8
    datas[2][ 8] = texture_data(  32,     128,  70992056,     128); // hash: e963c240d9a5dbc71e641bbc8fb40aed, width: 4, height: 4
    datas[2][ 9] = texture_data(  16,      32,  70992184,      32); // hash: 28ba0932c6bfe03072dc26e152d9aff9, width: 2, height: 2
    datas[2][10] = texture_data(   8,       8,  70992216,       8); // hash: e49d8ddf01bacca3e5c7a600bb6c2dcc, width: 1, height: 1
    datas[3][ 0] = texture_data(8192, 8388608,  70992224, 8388608); // hash: b6abe08e32d3e208681502545ec9a502, width: 1024, height: 1024
    datas[3][ 1] = texture_data(4096, 2097152,  79380832, 2097152); // hash: e6f295667f451f1dcd03c57c771a50f8, width: 512, height: 512
    datas[3][ 2] = texture_data(2048,  524288,  81477984,  524288); // hash: 9ba2bd3e24194414485c0ebc3e458501, width: 256, height: 256
    datas[3][ 3] = texture_data(1024,  131072,  82002272,  131072); // hash: 29ebe33d875685675874819ee8a818ca, width: 128, height: 128
    datas[3][ 4] = texture_data( 512,   32768,  82133344,   32768); // hash: 708965672b0d4e23bd2a05d474b625a3, width: 64, height: 64
    datas[3][ 5] = texture_data( 256,    8192,  82166112,    8192); // hash: 5f6d7d4ca99629b1c15e7ef499559efd, width: 32, height: 32
    datas[3][ 6] = texture_data( 128,    2048,  82174304,    2048); // hash: 98c2cba26a372c4e766eb30171b1dd8b, width: 16, height: 16
    datas[3][ 7] = texture_data(  64,     512,  82176352,     512); // hash: 0f469f10c756deff3bc3d6738410532b, width: 8, height: 8
    datas[3][ 8] = texture_data(  32,     128,  82176864,     128); // hash: b60a19c3b4ffcc58ea798a0e15118275, width: 4, height: 4
    datas[3][ 9] = texture_data(  16,      32,  82176992,      32); // hash: 713724c115e1bd4901def7feabb124c7, width: 2, height: 2
    datas[3][10] = texture_data(   8,       8,  82177024,       8); // hash: b04fe0e7106782ae1102d31cd21ed5b6, width: 1, height: 1
    datas[4][ 0] = texture_data(8192, 8388608,  82177032, 8388608); // hash: b33270086b73324a5eaed477b1b03a6b, width: 1024, height: 1024
    datas[4][ 1] = texture_data(4096, 2097152,  90565640, 2097152); // hash: eb71724ac6de7c410c611c34e93379d8, width: 512, height: 512
    datas[4][ 2] = texture_data(2048,  524288,  92662792,  524288); // hash: 3496fe103c6019cb8cb6fdb9e085af48, width: 256, height: 256
    datas[4][ 3] = texture_data(1024,  131072,  93187080,  131072); // hash: de2f2609b7d4158cb88f84f64901a3ef, width: 128, height: 128
    datas[4][ 4] = texture_data( 512,   32768,  93318152,   32768); // hash: 96947b3f2ea620a01d6fdff092912a55, width: 64, height: 64
    datas[4][ 5] = texture_data( 256,    8192,  93350920,    8192); // hash: 5a3747d336e986a8b13f8130e9810f82, width: 32, height: 32
    datas[4][ 6] = texture_data( 128,    2048,  93359112,    2048); // hash: 0a43e3945958925c7202dc7bed7a28fb, width: 16, height: 16
    datas[4][ 7] = texture_data(  64,     512,  93361160,     512); // hash: f2bec3deca649d33f94e5d50c34c34b0, width: 8, height: 8
    datas[4][ 8] = texture_data(  32,     128,  93361672,     128); // hash: 838b85ee55cd58cd173c84757b13a289, width: 4, height: 4
    datas[4][ 9] = texture_data(  16,      32,  93361800,      32); // hash: 4468b4120baf512adced842a40041405, width: 2, height: 2
    datas[4][10] = texture_data(   8,       8,  93361832,       8); // hash: ea9cf4b87e3051e5a1a80b458f58df15, width: 1, height: 1
    datas[5][ 0] = texture_data(8192, 8388608,  93361840, 8388608); // hash: 7e43c9b640bbf8113099bc1f54a3f4d0, width: 1024, height: 1024
    datas[5][ 1] = texture_data(4096, 2097152, 101750448, 2097152); // hash: 043f9e4a7880bb4bf92ecfd1f8bfe6e1, width: 512, height: 512
    datas[5][ 2] = texture_data(2048,  524288, 103847600,  524288); // hash: fb6cc9718402d5adbfb1b1ed2b777bf9, width: 256, height: 256
    datas[5][ 3] = texture_data(1024,  131072, 104371888,  131072); // hash: ac11b78e531199f5a52bb19c06bf65e3, width: 128, height: 128
    datas[5][ 4] = texture_data( 512,   32768, 104502960,   32768); // hash: 4de8799a3aed4c8d8b9bd9b0f983e1e9, width: 64, height: 64
    datas[5][ 5] = texture_data( 256,    8192, 104535728,    8192); // hash: 592488bca93776e60489232ba93b9415, width: 32, height: 32
    datas[5][ 6] = texture_data( 128,    2048, 104543920,    2048); // hash: 6a47f7e57de4b65a3b6f8441291d4274, width: 16, height: 16
    datas[5][ 7] = texture_data(  64,     512, 104545968,     512); // hash: 4f8116ee9953a5d394c961a00058f844, width: 8, height: 8
    datas[5][ 8] = texture_data(  32,     128, 104546480,     128); // hash: 79e1654473d8ba7d59667cfe71798ab4, width: 4, height: 4
    datas[5][ 9] = texture_data(  16,      32, 104546608,      32); // hash: 2b12f35cb941ce5444d6fe010c71e513, width: 2, height: 2
    datas[5][10] = texture_data(   8,       8, 104546640,       8); // hash: 424dd03dbe2eb284de8a0348871d0704, width: 1, height: 1
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
    datas[ 0] = texture_data(2048, 1048576, 104546648, 1048576); // hash: 7f7048fcf260a2aa1eae75b62340d74d, width: 512, height: 512
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
    datas[0][ 0] = texture_data(1024,  131072, 105595224,  131072); // hash: 2e2aa256fff67123a2a4fc3322c42d71, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072, 105726296,  131072); // hash: 3bbc601120883a26ccd37f0a530c0b12, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072, 105857368,  131072); // hash: 45b9b9637ea6e7204764acb1353843db, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072, 105988440,  131072); // hash: d4983dea763c4559d2c168976b5f778d, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072, 106119512,  131072); // hash: 14afe16f16de2e066ebd895b3d834142, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072, 106250584,  131072); // hash: 97d8fac08f450c25bcaa17110f0fa7e9, width: 128, height: 128
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
    datas[0][ 0] = texture_data(8192, 8388608, 106381656, 8388608); // hash: f1dac2f642724068cfe5a596095cd1af, width: 1024, height: 1024
    datas[0][ 1] = texture_data(4096, 2097152, 114770264, 2097152); // hash: 1c8a26b6a21b45ca2e5a85c6f40427de, width: 512, height: 512
    datas[0][ 2] = texture_data(2048,  524288, 116867416,  524288); // hash: 4d6f669c74778027f6d1906e39178e63, width: 256, height: 256
    datas[0][ 3] = texture_data(1024,  131072, 117391704,  131072); // hash: c4bc3c5d04987935544004eecea8af9a, width: 128, height: 128
    datas[0][ 4] = texture_data( 512,   32768, 117522776,   32768); // hash: 86600ba136ef9907f544b129205ab843, width: 64, height: 64
    datas[0][ 5] = texture_data( 256,    8192, 117555544,    8192); // hash: 54166bee57fd14547e55dcdd21fc8d95, width: 32, height: 32
    datas[0][ 6] = texture_data( 128,    2048, 117563736,    2048); // hash: da8f24bc0157eb77c104242d1709cef9, width: 16, height: 16
    datas[0][ 7] = texture_data(  64,     512, 117565784,     512); // hash: 225f6df30d026df02eba6cc2adbf5275, width: 8, height: 8
    datas[0][ 8] = texture_data(  32,     128, 117566296,     128); // hash: f087a791fb8f4e1347d275d1ee3faf5d, width: 4, height: 4
    datas[0][ 9] = texture_data(  16,      32, 117566424,      32); // hash: 6d1e1eb8148cc2d9a44918a3ca1f827d, width: 2, height: 2
    datas[0][10] = texture_data(   8,       8, 117566456,       8); // hash: 9b6001f2a75c12628d93a3bf0e10d296, width: 1, height: 1
    datas[1][ 0] = texture_data(8192, 8388608, 117566464, 8388608); // hash: 5c262cb25a1e21e5fc9c9e2aacf369b9, width: 1024, height: 1024
    datas[1][ 1] = texture_data(4096, 2097152, 125955072, 2097152); // hash: dc6de75243f0b0a4742616ada7d91ae8, width: 512, height: 512
    datas[1][ 2] = texture_data(2048,  524288, 128052224,  524288); // hash: aeccbe3baca29ea206d9191f20c37e5d, width: 256, height: 256
    datas[1][ 3] = texture_data(1024,  131072, 128576512,  131072); // hash: c41533f8d83dac3f0fbeaff88dc27024, width: 128, height: 128
    datas[1][ 4] = texture_data( 512,   32768, 128707584,   32768); // hash: abe0b4168f2754fe09ad90f9a50f6d37, width: 64, height: 64
    datas[1][ 5] = texture_data( 256,    8192, 128740352,    8192); // hash: 7999d6c0343180d3b23ee37de3cb720b, width: 32, height: 32
    datas[1][ 6] = texture_data( 128,    2048, 128748544,    2048); // hash: 5b6abcb6d2100b59fe8f2f81e203e85f, width: 16, height: 16
    datas[1][ 7] = texture_data(  64,     512, 128750592,     512); // hash: fb70262f76033cc99248d83f46de534e, width: 8, height: 8
    datas[1][ 8] = texture_data(  32,     128, 128751104,     128); // hash: 54d914303dd5eefbefcce3f4c58922ac, width: 4, height: 4
    datas[1][ 9] = texture_data(  16,      32, 128751232,      32); // hash: 26f4fdc1ccac45caa1509b57db7912a3, width: 2, height: 2
    datas[1][10] = texture_data(   8,       8, 128751264,       8); // hash: a70b8d4ec200454162775b5f8b1ec109, width: 1, height: 1
    datas[2][ 0] = texture_data(8192, 8388608, 128751272, 8388608); // hash: 655df6523b827f552d2e77a9b26656cf, width: 1024, height: 1024
    datas[2][ 1] = texture_data(4096, 2097152, 137139880, 2097152); // hash: 36be9339496f43f7eef083712d0a26d4, width: 512, height: 512
    datas[2][ 2] = texture_data(2048,  524288, 139237032,  524288); // hash: 80897a861e3de3a02552a919606c69ef, width: 256, height: 256
    datas[2][ 3] = texture_data(1024,  131072, 139761320,  131072); // hash: 6086480b26d0b7ce046f1ca6d5ef8693, width: 128, height: 128
    datas[2][ 4] = texture_data( 512,   32768, 139892392,   32768); // hash: 11a09eaf78b226c2fe91a2b32a47d9bc, width: 64, height: 64
    datas[2][ 5] = texture_data( 256,    8192, 139925160,    8192); // hash: 71213a81faa787f7fb9b4d5ed51732b7, width: 32, height: 32
    datas[2][ 6] = texture_data( 128,    2048, 139933352,    2048); // hash: 4af1ccf6f5c0a8fabff467c323581bcd, width: 16, height: 16
    datas[2][ 7] = texture_data(  64,     512, 139935400,     512); // hash: 44594d2e62e9535b5be0f465f8536a46, width: 8, height: 8
    datas[2][ 8] = texture_data(  32,     128, 139935912,     128); // hash: b42139ed8d524ec18340525524b69e77, width: 4, height: 4
    datas[2][ 9] = texture_data(  16,      32, 139936040,      32); // hash: 8284584e26ed0c92b5d71c5d6ff3bf9d, width: 2, height: 2
    datas[2][10] = texture_data(   8,       8, 139936072,       8); // hash: aec97c034320ee9b0b2a0dd4d5171e60, width: 1, height: 1
    datas[3][ 0] = texture_data(8192, 8388608, 139936080, 8388608); // hash: 95b2e9256e59cae29c48f2d7d4ede70b, width: 1024, height: 1024
    datas[3][ 1] = texture_data(4096, 2097152, 148324688, 2097152); // hash: bdcbdc9f5021c87bc74cc2e79620c711, width: 512, height: 512
    datas[3][ 2] = texture_data(2048,  524288, 150421840,  524288); // hash: 8f597032a29f04cc7c266b1473189190, width: 256, height: 256
    datas[3][ 3] = texture_data(1024,  131072, 150946128,  131072); // hash: a522798ddbace386b9748c650d0bd589, width: 128, height: 128
    datas[3][ 4] = texture_data( 512,   32768, 151077200,   32768); // hash: b39d74423230d4ef34f4a01354f06dbb, width: 64, height: 64
    datas[3][ 5] = texture_data( 256,    8192, 151109968,    8192); // hash: 16b172ecdc77d53f3f4f910499416e22, width: 32, height: 32
    datas[3][ 6] = texture_data( 128,    2048, 151118160,    2048); // hash: 8cd998d0b667d36d9d17a7db82b30a88, width: 16, height: 16
    datas[3][ 7] = texture_data(  64,     512, 151120208,     512); // hash: f39fa59576ca888db963623568bf0ea5, width: 8, height: 8
    datas[3][ 8] = texture_data(  32,     128, 151120720,     128); // hash: 2add947eb14fada697697931f3a250c1, width: 4, height: 4
    datas[3][ 9] = texture_data(  16,      32, 151120848,      32); // hash: b2a1d97d48c8032f37cf36ae9cb35ea4, width: 2, height: 2
    datas[3][10] = texture_data(   8,       8, 151120880,       8); // hash: 563b13464e49eab1a96147576b00c6bc, width: 1, height: 1
    datas[4][ 0] = texture_data(8192, 8388608, 151120888, 8388608); // hash: 17a3f09f6461513ab3978b63fab6b79d, width: 1024, height: 1024
    datas[4][ 1] = texture_data(4096, 2097152, 159509496, 2097152); // hash: 5dfa489e2f014ebff7c26136bf7930e3, width: 512, height: 512
    datas[4][ 2] = texture_data(2048,  524288, 161606648,  524288); // hash: d30d23f72d1d2872a6fbb177c397315f, width: 256, height: 256
    datas[4][ 3] = texture_data(1024,  131072, 162130936,  131072); // hash: 7ed58d6f690b7d7d1145fb9a1844a2f1, width: 128, height: 128
    datas[4][ 4] = texture_data( 512,   32768, 162262008,   32768); // hash: 4fd24eb9b5086d7b02bf0cb6680cb56a, width: 64, height: 64
    datas[4][ 5] = texture_data( 256,    8192, 162294776,    8192); // hash: 20cd0518fb2e562f63cc76472572af74, width: 32, height: 32
    datas[4][ 6] = texture_data( 128,    2048, 162302968,    2048); // hash: fe758f7b4f6cdd6ce36f3b63bbccebb4, width: 16, height: 16
    datas[4][ 7] = texture_data(  64,     512, 162305016,     512); // hash: 573f4f668a5c39117daee74b730bd470, width: 8, height: 8
    datas[4][ 8] = texture_data(  32,     128, 162305528,     128); // hash: 1704840cfb6d56378118f39d7378bf49, width: 4, height: 4
    datas[4][ 9] = texture_data(  16,      32, 162305656,      32); // hash: a226a62cb18192d068f68dea3b3c2d05, width: 2, height: 2
    datas[4][10] = texture_data(   8,       8, 162305688,       8); // hash: 16be76e995343fb04ff0c00918a40e90, width: 1, height: 1
    datas[5][ 0] = texture_data(8192, 8388608, 162305696, 8388608); // hash: 4b3393b50e560bef692124e8d132134d, width: 1024, height: 1024
    datas[5][ 1] = texture_data(4096, 2097152, 170694304, 2097152); // hash: 1eef69956e198f5357901dd07f464a6d, width: 512, height: 512
    datas[5][ 2] = texture_data(2048,  524288, 172791456,  524288); // hash: 1af3c6af6280f1792a657deb5a923955, width: 256, height: 256
    datas[5][ 3] = texture_data(1024,  131072, 173315744,  131072); // hash: 23fc8e43c4bbe4ddef07fc5fd6bd5e85, width: 128, height: 128
    datas[5][ 4] = texture_data( 512,   32768, 173446816,   32768); // hash: dc8659af1986cd692f12566594ae6da1, width: 64, height: 64
    datas[5][ 5] = texture_data( 256,    8192, 173479584,    8192); // hash: a101bf90ff26cdbab7d36ae740f5416e, width: 32, height: 32
    datas[5][ 6] = texture_data( 128,    2048, 173487776,    2048); // hash: e43e67959b0bbe90014604b650cbac75, width: 16, height: 16
    datas[5][ 7] = texture_data(  64,     512, 173489824,     512); // hash: 18701ec219c94b89f728866e3b401cb8, width: 8, height: 8
    datas[5][ 8] = texture_data(  32,     128, 173490336,     128); // hash: 1a2d5e4bab40436129b0906c2540abb1, width: 4, height: 4
    datas[5][ 9] = texture_data(  16,      32, 173490464,      32); // hash: 0064208c729ac8dfa2c4c6a8dadbeea9, width: 2, height: 2
    datas[5][10] = texture_data(   8,       8, 173490496,       8); // hash: 477b556f26425c0564e42f5471a29448, width: 1, height: 1
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
    datas[0][ 0] = texture_data(1024,  131072, 173490504,  131072); // hash: 14bace1323fdb1b8093dfd31df5652a4, width: 128, height: 128
    datas[1][ 0] = texture_data(1024,  131072, 173621576,  131072); // hash: be316532626188efaffe393800a2b463, width: 128, height: 128
    datas[2][ 0] = texture_data(1024,  131072, 173752648,  131072); // hash: 5f9bc366b6a4a7fb368796936dc50d94, width: 128, height: 128
    datas[3][ 0] = texture_data(1024,  131072, 173883720,  131072); // hash: 6f95ec7a8f2c6167a11879a2af8972d5, width: 128, height: 128
    datas[4][ 0] = texture_data(1024,  131072, 174014792,  131072); // hash: 0e69d9da1b68db7535563c39e2574521, width: 128, height: 128
    datas[5][ 0] = texture_data(1024,  131072, 174145864,  131072); // hash: c8dd89cfcc42f96602486d5c12a952c4, width: 128, height: 128
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
        .glyphs = transmuted_span<Glyph>(174276936, 94),
    };
}

auto Assets::roboto_medium_mesh_array() const -> MeshArray {
    // submesh_count: 94
    // vertex_count: 35628
    // face_count: 23448
    return MeshArray {
        // hash: 8384a220e7f3e90e627e9774b7dbd4c1
        .submeshes = transmuted_span<Submesh>(174279944, 94),
        // hash: 29374dddcd21f32bd4d1c96ff182a0ae
        .vertices = transmuted_span<Vertex>(174281072, 35628),
        // hash: 022ec4951ed71315112a36ddd2c7ecf2
        .indices = transmuted_span<Index>(175991216, 70344),
    };
}

Shaders::Shaders() {
    // hash: 11df2a6a14e273ed9fa8ae7e5da495d7
    _data = read_whole_file("fb_buffet_shaders.bin");
    FB_ASSERT(_data.size() == 149016);
}

// shader_hash: 5792fbf56668c072f9917d0d52e90c6d
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 64
// float_instruction_count: 12
// texture_load_instructions: 4
auto Shaders::cards_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 4860);
}

// shader_hash: 0277dbed47bb3692b278d7e759af728f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// texture_normal_instructions: 1
auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(4860, 4220);
}

// shader_hash: 0393b7b656e1c43b1a948cbf13bc87ed
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 94
// float_instruction_count: 20
// texture_load_instructions: 4
auto Shaders::crate_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9080, 5772);
}

// shader_hash: 9cdd6711b0d26d20ad1949c7c4a89050
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
auto Shaders::crate_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14852, 7992);
}

// shader_hash: bb318ca5796644e7a6c68671f7011d41
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 55
// float_instruction_count: 8
// texture_load_instructions: 1
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(22844, 4652);
}

// shader_hash: 4f92468768d022e91d050a21946f749f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 107
// float_instruction_count: 16
// texture_load_instructions: 3
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(27496, 5456);
}

// shader_hash: 1f79406f851cdc57fdd4846552b19811
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 58
// float_instruction_count: 17
// texture_normal_instructions: 1
// texture_comp_instructions: 1
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(32952, 5216);
}

// shader_hash: d7bde9ff419de59f4788f766651525ae
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 23
// float_instruction_count: 3
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(38168, 3748);
}

// shader_hash: 0820cea972058c552f588fd8ea7b324e
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 92
// float_instruction_count: 14
// texture_load_instructions: 3
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(41916, 5064);
}

// shader_hash: 785867d3fd40ec288756abc5b7fa1a8a
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// float_instruction_count: 13
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46980, 3228);
}

// shader_hash: 375a5653c5bb68d44e67412aa73d0ae3
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 636
// float_instruction_count: 188
// texture_load_instructions: 38
auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(50208, 10188);
}

// shader_hash: 3451eb25b73e7f9e7038c3afaf0e67a6
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 29
// float_instruction_count: 7
// texture_normal_instructions: 1
auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(60396, 4776);
}

// shader_hash: 38367bd0b1752e4ca0df66b75c2c3e42
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 32
// float_instruction_count: 4
// texture_load_instructions: 3
// texture_store_instructions: 3
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(65172, 4004);
}

// shader_hash: 27a8754335d1b1087f852a95afe6cf98
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 8
// texture_store_instructions: 1
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(69176, 3424);
}

// shader_hash: 6948df21d6f52241ffd77c4cc5ae1817
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
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(72600, 7328);
}

// shader_hash: 00bdae7a717783ec01c1587d7fe7f16b
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 90
// float_instruction_count: 14
// texture_load_instructions: 5
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(79928, 5488);
}

// shader_hash: 0d545b607d54aea303eb4a8db4cf31da
// input_parameters: 4
// output_parameters: 1
// instruction_count: 5
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(85416, 3144);
}

// shader_hash: 67ce76c1b8cef37afea87f3af4e63980
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 5
// instruction_count: 74
// float_instruction_count: 8
// texture_load_instructions: 3
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(88560, 5508);
}

// shader_hash: bf043e6ca1fc244f4228b30f10d7a497
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 5
// output_parameters: 1
// instruction_count: 105
// dynamic_flow_control_count: 2
// float_instruction_count: 40
// int_instruction_count: 4
// texture_load_instructions: 5
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(94068, 5928);
}

// shader_hash: 71d2c83dce694aa3e6dd4a98a244bf50
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(99996, 3256);
}

// shader_hash: fba11f278d24921065a7ff897de265ce
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 44
// float_instruction_count: 11
// texture_normal_instructions: 1
// texture_load_instructions: 1
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(103252, 4944);
}

// shader_hash: 96fd4851bbd7a6fbc23e845bb49648d9
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
auto Shaders::env_background_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(108196, 5172);
}

// shader_hash: 50d3f0f59d6642b0c1a799137c9be4c1
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 68
// dynamic_flow_control_count: 1
// float_instruction_count: 32
// int_instruction_count: 2
// texture_normal_instructions: 1
auto Shaders::env_background_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(113368, 5076);
}

// shader_hash: 12d711ae9b1f562b8dc9a6ed3dd5b1d2
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 65
// float_instruction_count: 8
// texture_load_instructions: 2
auto Shaders::env_model_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(118444, 5084);
}

// shader_hash: 0b1454f51be2a562bc4bf2f3e18f178d
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 124
// dynamic_flow_control_count: 1
// float_instruction_count: 66
// int_instruction_count: 1
// texture_normal_instructions: 3
auto Shaders::env_model_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(123528, 5588);
}

// shader_hash: 319ed77b707f15f69d8a8c1e177e13b3
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 62
// float_instruction_count: 6
// texture_load_instructions: 3
auto Shaders::text_background_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(129116, 5196);
}

// shader_hash: 585041d3c583a82abf55420ce670d565
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 26
// float_instruction_count: 5
// texture_normal_instructions: 1
auto Shaders::text_background_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(134312, 4736);
}

// shader_hash: 359c134277e6c9fb8d2a766f5bc2f2a0
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 97
// float_instruction_count: 14
// int_instruction_count: 1
// texture_load_instructions: 3
auto Shaders::text_glyph_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(139048, 5236);
}

// shader_hash: fd3ad7c5652657e033ffec4c9d8b1ef2
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 37
// float_instruction_count: 8
// texture_normal_instructions: 1
auto Shaders::text_glyph_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(144284, 4732);
}

#undef texture_data

} // namespace fb::baked::buffet
