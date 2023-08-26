#include "baked.hpp"

namespace fb::baked {

Assets::Assets() {
    _data = read_whole_file("fb_assets.bin");
    FB_ASSERT(_data.size() == 169830228);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        // hash: 799fc360204416196536a93c9eff68ae
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

auto Assets::heatmap_magma_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channel_count = 4,
        .mip_count = 9,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 256
                 // height: 1
                 .row_pitch = 1024,
                 .slice_pitch = 1024,
                 // hash: 33e2d2ee5bc875a573dd90c6f664a91c
                 .data = transmuted_span<std::byte>(162588, 1024),
             },
             TextureData {
                 // mip_level: 1
                 // width: 128
                 // height: 1
                 .row_pitch = 512,
                 .slice_pitch = 512,
                 // hash: 86cc43959a92ff04cedd3d3aa312eee8
                 .data = transmuted_span<std::byte>(163612, 512),
             },
             TextureData {
                 // mip_level: 2
                 // width: 64
                 // height: 1
                 .row_pitch = 256,
                 .slice_pitch = 256,
                 // hash: 734e2c3abf0ea59042a7a406af85f071
                 .data = transmuted_span<std::byte>(164124, 256),
             },
             TextureData {
                 // mip_level: 3
                 // width: 32
                 // height: 1
                 .row_pitch = 128,
                 .slice_pitch = 128,
                 // hash: 25c341c38486da6ac6ba118d8025af92
                 .data = transmuted_span<std::byte>(164380, 128),
             },
             TextureData {
                 // mip_level: 4
                 // width: 16
                 // height: 1
                 .row_pitch = 64,
                 .slice_pitch = 64,
                 // hash: 37ef2403766d2fac303b4bea97adf132
                 .data = transmuted_span<std::byte>(164508, 64),
             },
             TextureData {
                 // mip_level: 5
                 // width: 8
                 // height: 1
                 .row_pitch = 32,
                 .slice_pitch = 32,
                 // hash: bd143bc176b967ced1d48e4f8a4f2cb9
                 .data = transmuted_span<std::byte>(164572, 32),
             },
             TextureData {
                 // mip_level: 6
                 // width: 4
                 // height: 1
                 .row_pitch = 16,
                 .slice_pitch = 16,
                 // hash: 6ffa2da24c9dee7344309befeccb2190
                 .data = transmuted_span<std::byte>(164604, 16),
             },
             TextureData {
                 // mip_level: 7
                 // width: 2
                 // height: 1
                 .row_pitch = 8,
                 .slice_pitch = 8,
                 // hash: 5ebdc1722d8abe18ccc4c3a49ff06d72
                 .data = transmuted_span<std::byte>(164620, 8),
             },
             TextureData {
                 // mip_level: 8
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 448ab11a285422615f166b43c7d70bfb
                 .data = transmuted_span<std::byte>(164628, 4),
             }},
    };
}

auto Assets::heatmap_viridis_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channel_count = 4,
        .mip_count = 9,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 256
                 // height: 1
                 .row_pitch = 1024,
                 .slice_pitch = 1024,
                 // hash: 68bceaeffeb954c3304af1cdb4a1e5fc
                 .data = transmuted_span<std::byte>(164632, 1024),
             },
             TextureData {
                 // mip_level: 1
                 // width: 128
                 // height: 1
                 .row_pitch = 512,
                 .slice_pitch = 512,
                 // hash: 325545a90af39f65504becc272d17d1b
                 .data = transmuted_span<std::byte>(165656, 512),
             },
             TextureData {
                 // mip_level: 2
                 // width: 64
                 // height: 1
                 .row_pitch = 256,
                 .slice_pitch = 256,
                 // hash: 1872652d1b188d98960797a861a05c57
                 .data = transmuted_span<std::byte>(166168, 256),
             },
             TextureData {
                 // mip_level: 3
                 // width: 32
                 // height: 1
                 .row_pitch = 128,
                 .slice_pitch = 128,
                 // hash: 0defd6e02d84d1b2acf60921732d378e
                 .data = transmuted_span<std::byte>(166424, 128),
             },
             TextureData {
                 // mip_level: 4
                 // width: 16
                 // height: 1
                 .row_pitch = 64,
                 .slice_pitch = 64,
                 // hash: 92294ed3ea02ed1f4319cd20bf3f71c6
                 .data = transmuted_span<std::byte>(166552, 64),
             },
             TextureData {
                 // mip_level: 5
                 // width: 8
                 // height: 1
                 .row_pitch = 32,
                 .slice_pitch = 32,
                 // hash: 15fcf5da43414ee9b17fc9d98ce88bb0
                 .data = transmuted_span<std::byte>(166616, 32),
             },
             TextureData {
                 // mip_level: 6
                 // width: 4
                 // height: 1
                 .row_pitch = 16,
                 .slice_pitch = 16,
                 // hash: 844ea3b742c5f7a9dd087f275be89521
                 .data = transmuted_span<std::byte>(166648, 16),
             },
             TextureData {
                 // mip_level: 7
                 // width: 2
                 // height: 1
                 .row_pitch = 8,
                 .slice_pitch = 8,
                 // hash: 40a6faa288fcc089d080e840b745b320
                 .data = transmuted_span<std::byte>(166664, 8),
             },
             TextureData {
                 // mip_level: 8
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 7a1c709b7c6639090826f10a8cfc7dab
                 .data = transmuted_span<std::byte>(166672, 4),
             }},
    };
}

auto Assets::sci_fi_case_mesh() const -> Mesh {
    return Mesh {
        // hash: 576ddd3553b833a1778d067f0cfc0f20
        .vertices = transmuted_span<Vertex>(166676, 2025),
        // hash: b4d34f8bbc09c51115542a8b94aec26a
        .indices = transmuted_span<Index>(263876, 5301),
    };
}

auto Assets::sci_fi_case_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: 1c0cad906e975f6b2b0f9d24de7c4926
                 .data = transmuted_span<std::byte>(285080, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: 0fc5f72ca36e69f9ee7aa3e1007128da
                 .data = transmuted_span<std::byte>(4479384, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 5a31a9aea69b9572403af8c398ff1bb1
                 .data = transmuted_span<std::byte>(5527960, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: a7885b770b2e31e7cc2b44a128f01c11
                 .data = transmuted_span<std::byte>(5790104, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: 57315ba87386a5f4d7792ac48fafbcc2
                 .data = transmuted_span<std::byte>(5855640, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: a58e3210388634d7155f2901e7501649
                 .data = transmuted_span<std::byte>(5872024, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: 148fb23c5ce094c10a5185b87ded78f6
                 .data = transmuted_span<std::byte>(5876120, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: 2b29afab47c66cc6470d9fd805320d9c
                 .data = transmuted_span<std::byte>(5877144, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 618df3e3abd8ce160684dd0e4549ce74
                 .data = transmuted_span<std::byte>(5877400, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 774155eac0ec01f795a1255477bebbb4
                 .data = transmuted_span<std::byte>(5877464, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: f27ac865f3272414b83c54340f946f64
                 .data = transmuted_span<std::byte>(5877480, 4),
             }},
    };
}

auto Assets::sci_fi_case_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: d5c691d45d7f61290eb8b12ed954e8f1
                 .data = transmuted_span<std::byte>(5877484, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: 8bd2e1f3f4407531cc8243c08e556004
                 .data = transmuted_span<std::byte>(10071788, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 65ad91e067821102e44d6e3811da1611
                 .data = transmuted_span<std::byte>(11120364, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: 01920ca08158b9166b3b5f99ea99a101
                 .data = transmuted_span<std::byte>(11382508, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: ab34727e8f142779cfe5eedc131212a3
                 .data = transmuted_span<std::byte>(11448044, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: dfe0b70bf322d90fb2bff80c5ab1d8c9
                 .data = transmuted_span<std::byte>(11464428, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: b3aa202e184073b05965c82d4e60ec78
                 .data = transmuted_span<std::byte>(11468524, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: a592dfd5855b6ee57b42d9edfa7f2833
                 .data = transmuted_span<std::byte>(11469548, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: c3439bbdf4acc22b41baf1a96203e800
                 .data = transmuted_span<std::byte>(11469804, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 17e7ca5da0e3c69176067d4cf9dff17a
                 .data = transmuted_span<std::byte>(11469868, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: f334417f8373d6f0ef2e2f2651e34647
                 .data = transmuted_span<std::byte>(11469884, 4),
             }},
    };
}

auto Assets::sci_fi_case_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: ed04eb3dfe14725c0ad06ee90e903d7b
                 .data = transmuted_span<std::byte>(11469888, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: bef9846f192c1a321592fe54f565ab64
                 .data = transmuted_span<std::byte>(15664192, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 3114a59afa1940f13944b1903aecca77
                 .data = transmuted_span<std::byte>(16712768, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: af552342df2d95a4e8db91c262d030ad
                 .data = transmuted_span<std::byte>(16974912, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: ae6b271b6cedc3117259743503987475
                 .data = transmuted_span<std::byte>(17040448, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: 803f12a9da7dc4117b935a6570eb6e62
                 .data = transmuted_span<std::byte>(17056832, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: 2de9c42bfe1107d80ba68be92c4133dd
                 .data = transmuted_span<std::byte>(17060928, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: d8a8fba03904f3e9c7123cb6b038fece
                 .data = transmuted_span<std::byte>(17061952, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 5361feba23fce4703e3c634048c080d7
                 .data = transmuted_span<std::byte>(17062208, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: f2b6d357693a6c96c8686a98bd80974b
                 .data = transmuted_span<std::byte>(17062272, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: a67c136df57f927e6e07eab72b4f022b
                 .data = transmuted_span<std::byte>(17062288, 4),
             }},
    };
}

auto Assets::metal_plane_mesh() const -> Mesh {
    return Mesh {
        // hash: 7513db2c165692b22a3bb4374738dab7
        .vertices = transmuted_span<Vertex>(17062292, 4),
        // hash: 96880b352a2cd08ebe7a559eac217606
        .indices = transmuted_span<Index>(17062484, 6),
    };
}

auto Assets::metal_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: b01e0981487f9386f14d7e555623c2ba
                 .data = transmuted_span<std::byte>(17062508, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: 75c211006262e9722c80d57b27242a91
                 .data = transmuted_span<std::byte>(21256812, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 000fa85c2ab807e76e30458d71683c52
                 .data = transmuted_span<std::byte>(22305388, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: 5785c050ddf763147be0e8a10f2fd28b
                 .data = transmuted_span<std::byte>(22567532, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: 257a6272975283c2ba3982e0f48f3871
                 .data = transmuted_span<std::byte>(22633068, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: f3e2a564cd47f149462ab82d010ac6fc
                 .data = transmuted_span<std::byte>(22649452, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: 043e70e3d266bc8fa1af4ad7203787c9
                 .data = transmuted_span<std::byte>(22653548, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: 144abce73f4bb6a7748614fffa83c5cd
                 .data = transmuted_span<std::byte>(22654572, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: de1352b64f46a4757900083a9975b5a4
                 .data = transmuted_span<std::byte>(22654828, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 6d587cac4b377cdce6b029d7b6aa9ed6
                 .data = transmuted_span<std::byte>(22654892, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 275847d06c6d2099829491e6f3f23a61
                 .data = transmuted_span<std::byte>(22654908, 4),
             }},
    };
}

auto Assets::metal_plane_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: 2c3220bc625827e8a3a07df006fd2577
                 .data = transmuted_span<std::byte>(22654912, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: fc8ccb58611836d0fe177ea6bb751f8c
                 .data = transmuted_span<std::byte>(26849216, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: c79ba2ff35a7bea09af7cedce7153cb6
                 .data = transmuted_span<std::byte>(27897792, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: 861b5f55bc01af94eccb54fadede533a
                 .data = transmuted_span<std::byte>(28159936, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: c6241fb22bbdb204055a36c59e19eaa7
                 .data = transmuted_span<std::byte>(28225472, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: 96a28968bbeb40fd031cca35a218225c
                 .data = transmuted_span<std::byte>(28241856, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: c588a82ede6113e9a066f3ae10347b68
                 .data = transmuted_span<std::byte>(28245952, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: 23f1fc279c052f32433e694e9dcf34d8
                 .data = transmuted_span<std::byte>(28246976, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: cfc2177f15b55d6e9cd459a7ec5de27c
                 .data = transmuted_span<std::byte>(28247232, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 56c2be73c379af90ab87d34bc436619b
                 .data = transmuted_span<std::byte>(28247296, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 01862aadb9ce1ab37fe3803a108ced53
                 .data = transmuted_span<std::byte>(28247312, 4),
             }},
    };
}

auto Assets::metal_plane_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 11,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 1024
                 // height: 1024
                 .row_pitch = 4096,
                 .slice_pitch = 4194304,
                 // hash: a0a8987101c3763d88cded178f004edc
                 .data = transmuted_span<std::byte>(28247316, 4194304),
             },
             TextureData {
                 // mip_level: 1
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: 703b054f217ed2a761781a5790cc9da1
                 .data = transmuted_span<std::byte>(32441620, 1048576),
             },
             TextureData {
                 // mip_level: 2
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 2fdd37d099fbc0a45280294c7e26bda2
                 .data = transmuted_span<std::byte>(33490196, 262144),
             },
             TextureData {
                 // mip_level: 3
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: 52a2e110668972c369b81ca2f7d03732
                 .data = transmuted_span<std::byte>(33752340, 65536),
             },
             TextureData {
                 // mip_level: 4
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: 8df31f4575f5e3b5bdeb96cc013a2505
                 .data = transmuted_span<std::byte>(33817876, 16384),
             },
             TextureData {
                 // mip_level: 5
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: f06fd168844ac517f257d6cde21e3c3e
                 .data = transmuted_span<std::byte>(33834260, 4096),
             },
             TextureData {
                 // mip_level: 6
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: fd4f859331efb591100afda7bd764091
                 .data = transmuted_span<std::byte>(33838356, 1024),
             },
             TextureData {
                 // mip_level: 7
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: e5c5824300a76bb01e0e389a992d2eb8
                 .data = transmuted_span<std::byte>(33839380, 256),
             },
             TextureData {
                 // mip_level: 8
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 0f88755d2cf041ee01b24e89928c89f4
                 .data = transmuted_span<std::byte>(33839636, 64),
             },
             TextureData {
                 // mip_level: 9
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: f77b6b506c3f744884e3d267355042be
                 .data = transmuted_span<std::byte>(33839700, 16),
             },
             TextureData {
                 // mip_level: 10
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: eb09f6f496892ae3a96abfd9da49d00b
                 .data = transmuted_span<std::byte>(33839716, 4),
             }},
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    return Mesh {
        // hash: cee14e1c21b8adac5f904691e307303a
        .vertices = transmuted_span<Vertex>(33839720, 725),
        // hash: 2e21503f47bdce31841a1d7890743c68
        .indices = transmuted_span<Index>(33874520, 2034),
    };
}

auto Assets::coconut_tree_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .channel_count = 4,
        .mip_count = 10,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 512
                 // height: 512
                 .row_pitch = 2048,
                 .slice_pitch = 1048576,
                 // hash: 61b50bca3a05bbee4e3297d007f5fc25
                 .data = transmuted_span<std::byte>(33882656, 1048576),
             },
             TextureData {
                 // mip_level: 1
                 // width: 256
                 // height: 256
                 .row_pitch = 1024,
                 .slice_pitch = 262144,
                 // hash: 0edae200d9d108428bd9b503d8169fbf
                 .data = transmuted_span<std::byte>(34931232, 262144),
             },
             TextureData {
                 // mip_level: 2
                 // width: 128
                 // height: 128
                 .row_pitch = 512,
                 .slice_pitch = 65536,
                 // hash: 594493b0020d694430b571fc79867452
                 .data = transmuted_span<std::byte>(35193376, 65536),
             },
             TextureData {
                 // mip_level: 3
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: 7c277a922339cb6f8cee9b7d4fb0d456
                 .data = transmuted_span<std::byte>(35258912, 16384),
             },
             TextureData {
                 // mip_level: 4
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: 2251821b2955fa891380f4eac15bcd6c
                 .data = transmuted_span<std::byte>(35275296, 4096),
             },
             TextureData {
                 // mip_level: 5
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: 5b943a736dfa82072e5cb6affe649865
                 .data = transmuted_span<std::byte>(35279392, 1024),
             },
             TextureData {
                 // mip_level: 6
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: c842b2708e4811562e58fa88d52283be
                 .data = transmuted_span<std::byte>(35280416, 256),
             },
             TextureData {
                 // mip_level: 7
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 4b46d1e4178e56188b4c00c7fc23cb42
                 .data = transmuted_span<std::byte>(35280672, 64),
             },
             TextureData {
                 // mip_level: 8
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 570f797ebb674c8e914531a49491f97f
                 .data = transmuted_span<std::byte>(35280736, 16),
             },
             TextureData {
                 // mip_level: 9
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 68d5e1ca0194c1023749ac2880a275c9
                 .data = transmuted_span<std::byte>(35280752, 4),
             }},
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    return Mesh {
        // hash: 8c75d3a9d4a8e7e3a9a7e30729dcb460
        .vertices = transmuted_span<Vertex>(35280756, 4),
        // hash: 96880b352a2cd08ebe7a559eac217606
        .indices = transmuted_span<Index>(35280948, 6),
    };
}

auto Assets::sand_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .channel_count = 4,
        .mip_count = 7,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 64
                 // height: 64
                 .row_pitch = 256,
                 .slice_pitch = 16384,
                 // hash: cd9c6abea5eb3cc9b5846ab373f9d5d8
                 .data = transmuted_span<std::byte>(35280972, 16384),
             },
             TextureData {
                 // mip_level: 1
                 // width: 32
                 // height: 32
                 .row_pitch = 128,
                 .slice_pitch = 4096,
                 // hash: da49ee29170ccee71bde3a122fb1ae75
                 .data = transmuted_span<std::byte>(35297356, 4096),
             },
             TextureData {
                 // mip_level: 2
                 // width: 16
                 // height: 16
                 .row_pitch = 64,
                 .slice_pitch = 1024,
                 // hash: 63e53ce8aff7a307f0c1097ccf5d80c5
                 .data = transmuted_span<std::byte>(35301452, 1024),
             },
             TextureData {
                 // mip_level: 3
                 // width: 8
                 // height: 8
                 .row_pitch = 32,
                 .slice_pitch = 256,
                 // hash: 00f8f27693c1fe15fbdd5456945dfa9f
                 .data = transmuted_span<std::byte>(35302476, 256),
             },
             TextureData {
                 // mip_level: 4
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 5743429ba6b07a05fffee7cf591b7cc5
                 .data = transmuted_span<std::byte>(35302732, 64),
             },
             TextureData {
                 // mip_level: 5
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: c32f5c05e144ea6a196dd524d8dae576
                 .data = transmuted_span<std::byte>(35302796, 16),
             },
             TextureData {
                 // mip_level: 6
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 8005525e757dc43ff5d1826186e49ea5
                 .data = transmuted_span<std::byte>(35302812, 4),
             }},
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        // hash: e65e9d279ecf4a2eac69f51658a72576
        .skinning_vertices = transmuted_span<SkinningVertex>(35302816, 2430),
        // hash: fd12995a38e89146d53c7edd4f6cd3d0
        .indices = transmuted_span<Index>(35497216, 3102),
        // hash: d730e286c5631c1fdfa6f0f7191ddd3b
        .joint_nodes = transmuted_span<uint32_t>(35509624, 63),
        // hash: ee0ff01354a210fae3930c7992929c24
        .joint_inverse_binds = transmuted_span<Float4x4>(35509876, 63),
        // hash: e95d90faa9a34f6172c2c7b88a91046c
        .node_parents = transmuted_span<uint32_t>(35513908, 72),
        // hash: a3ce072a99fe8139419ff55378991f51
        .node_transforms = transmuted_span<Float4x4>(35514196, 72),
        // hash: 2ba2f2a43abbd5968d77adad334f5682
        .node_channels = transmuted_span<AnimationChannel>(35518804, 72),
        // hash: 3b153cfb6ae1bff95aa6dce90c05c435
        .node_channels_times_t = transmuted_span<float>(35522260, 2256),
        // hash: c2293a9e0414fc79c6d7cd00d4c1ba87
        .node_channels_times_r = transmuted_span<float>(35531284, 2274),
        // hash: 4d32d6eb8afd6df010a01af960891484
        .node_channels_times_s = transmuted_span<float>(35540380, 369),
        // hash: 1defd6ee1077f6a108951a438abf4c61
        .node_channels_values_t = transmuted_span<Float3>(35541856, 2256),
        // hash: f00545a5efeaef5e3e8a97a62d2ae87a
        .node_channels_values_r = transmuted_span<Quaternion>(35568928, 2274),
        // hash: f56ee25392a3504e5a600f0f847638e4
        .node_channels_values_s = transmuted_span<Float3>(35605312, 369),
    };
}

auto Assets::raccoon_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 4,
        .height = 4,
        .channel_count = 4,
        .mip_count = 3,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: f46411895f74b52626f359cce1525b8e
                 .data = transmuted_span<std::byte>(35609740, 64),
             },
             TextureData {
                 // mip_level: 1
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 937e68bc91d5794588f960b38a352b2d
                 .data = transmuted_span<std::byte>(35609804, 16),
             },
             TextureData {
                 // mip_level: 2
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 44cbb8d6aae19256febe1555bfc8a1a1
                 .data = transmuted_span<std::byte>(35609820, 4),
             }},
    };
}

auto Assets::raccoon_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 4,
        .height = 4,
        .channel_count = 4,
        .mip_count = 3,
        .datas =
            {TextureData {
                 // mip_level: 0
                 // width: 4
                 // height: 4
                 .row_pitch = 16,
                 .slice_pitch = 64,
                 // hash: 22771420d888fb100e7450610145f45d
                 .data = transmuted_span<std::byte>(35609824, 64),
             },
             TextureData {
                 // mip_level: 1
                 // width: 2
                 // height: 2
                 .row_pitch = 8,
                 .slice_pitch = 16,
                 // hash: 0eea1d73e7ab0514131b39b1aad79493
                 .data = transmuted_span<std::byte>(35609888, 16),
             },
             TextureData {
                 // mip_level: 2
                 // width: 1
                 // height: 1
                 .row_pitch = 4,
                 .slice_pitch = 4,
                 // hash: 44ff0e0fb6c73dbbd0b61d69281ee072
                 .data = transmuted_span<std::byte>(35609904, 4),
             }},
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    return Mesh {
        // hash: d83d3ba89f1591682879b254506b0400
        .vertices = transmuted_span<Vertex>(35609908, 24),
        // hash: 616e076015f03b0288fd27d32b7bf256
        .indices = transmuted_span<Index>(35611060, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    return Mesh {
        // hash: 952f25941581c525b7890e0284fd3b52
        .vertices = transmuted_span<Vertex>(35611204, 24),
        // hash: 0bbe6ec9d4b61b792981857b935d2c96
        .indices = transmuted_span<Index>(35612356, 36),
    };
}

auto Assets::winter_evening_cube_texture() const -> CubeTexture {
    return CubeTexture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 1024,
        .height = 1024,
        .channel_count = 4,
        .row_pitch = 16384,
        .slice_pitch = 16777216,
        .datas = std::to_array({
            // hash: 918af87792513fcb47721c6bb751b8ab
            transmuted_span<std::byte>(35612500, 16777216),
            // hash: ea87830a0e05072d855f1850fd3f22b1
            transmuted_span<std::byte>(52389716, 16777216),
            // hash: da32c3251997b5da09b8f00ed97cfe1d
            transmuted_span<std::byte>(69166932, 16777216),
            // hash: 8181516c3622f6119a276a7e317fe2d5
            transmuted_span<std::byte>(85944148, 16777216),
            // hash: 38d65d00389eb7511870cb5439ea27c0
            transmuted_span<std::byte>(102721364, 16777216),
            // hash: 49f7ddd4345a3fe231d2f04a4d7074ef
            transmuted_span<std::byte>(119498580, 16777216),
        }),
    };
}

auto Assets::farm_field_hdr_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 2048,
        .height = 1024,
        .channel_count = 4,
        .mip_count = 1,
        .datas = {TextureData {
            // mip_level: 0
            // width: 2048
            // height: 1024
            .row_pitch = 32768,
            .slice_pitch = 33554432,
            // hash: a842b81a2d1fa45a1b9debea55afd255
            .data = transmuted_span<std::byte>(136275796, 33554432),
        }},
    };
}

Shaders::Shaders() {
    _data = read_whole_file("fb_shaders.bin");
    FB_ASSERT(_data.size() == 149200);
}

// shader_hash: a2b037e929fa47e82668128ef69b5d3c
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 3
// instruction_count: 86
// float_instruction_count: 31
// int_instruction_count: 1
// texture_load_instructions: 3
auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5352);
}

// shader_hash: 78391642d962dafe09fcb897ff0baef4
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 3
// output_parameters: 1
// instruction_count: 28
// float_instruction_count: 4
// texture_normal_instructions: 1
auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5352, 4416);
}

// shader_hash: 90b108a0f53edc3edf0fdbdf347b5003
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 2
// instruction_count: 74
// float_instruction_count: 16
// texture_load_instructions: 2
auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9768, 5120);
}

// shader_hash: 28a5f694ad5b0a73913b0a924d402ba0
// input_parameters: 2
// output_parameters: 1
// instruction_count: 9
auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14888, 3056);
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
    return std::span(_data).subspan(17944, 4860);
}

// shader_hash: 0277dbed47bb3692b278d7e759af728f
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// texture_normal_instructions: 1
auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(22804, 4220);
}

// shader_hash: 6529e616c34b9ecea94c7de26fa74cbd
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 1478
// temp_array_count: 2048
// dynamic_flow_control_count: 30
// array_instruction_count: 76
// float_instruction_count: 449
// int_instruction_count: 347
// uint_instruction_count: 78
// texture_normal_instructions: 4
// texture_load_instructions: 16
// barrier_instructions: 9
// interlocked_instructions: 1
// texture_store_instructions: 22
auto Shaders::spd_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(27024, 13864);
}

// shader_hash: 7dc078eb58d093999da5027a16f28bb1
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 6
// instruction_count: 94
// float_instruction_count: 20
// texture_load_instructions: 4
auto Shaders::crate_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(40888, 5684);
}

// shader_hash: 01792c5cb7b41065df99d414271de589
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 6
// output_parameters: 1
// instruction_count: 282
// dynamic_flow_control_count: 1
// float_instruction_count: 189
// uint_instruction_count: 1
// texture_normal_instructions: 3
auto Shaders::crate_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46572, 7336);
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
    return std::span(_data).subspan(53908, 4652);
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
    return std::span(_data).subspan(58560, 5456);
}

// shader_hash: e662510b7848f5cc9d83ae16ddd08141
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 58
// float_instruction_count: 17
// texture_normal_instructions: 1
// texture_comp_instructions: 1
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(64016, 5216);
}

// shader_hash: d7bde9ff419de59f4788f766651525ae
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 23
// float_instruction_count: 3
// texture_load_instructions: 1
// texture_store_instructions: 1
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(69232, 3748);
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
    return std::span(_data).subspan(72980, 5064);
}

// shader_hash: 785867d3fd40ec288756abc5b7fa1a8a
// input_parameters: 2
// output_parameters: 1
// instruction_count: 20
// float_instruction_count: 13
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(78044, 3228);
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
    return std::span(_data).subspan(81272, 10188);
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
    return std::span(_data).subspan(91460, 4776);
}

// shader_hash: 38367bd0b1752e4ca0df66b75c2c3e42
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 32
// float_instruction_count: 4
// texture_load_instructions: 3
// texture_store_instructions: 3
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(96236, 4004);
}

// shader_hash: 27a8754335d1b1087f852a95afe6cf98
// constant_buffers: 1
// bound_resources: 1
// instruction_count: 8
// texture_store_instructions: 1
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(100240, 3424);
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
    return std::span(_data).subspan(103664, 7328);
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
    return std::span(_data).subspan(110992, 5488);
}

// shader_hash: 0d545b607d54aea303eb4a8db4cf31da
// input_parameters: 4
// output_parameters: 1
// instruction_count: 5
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(116480, 3144);
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
    return std::span(_data).subspan(119624, 5508);
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
    return std::span(_data).subspan(125132, 5928);
}

// shader_hash: 774cf19def534ea19fa2c04206753609
// input_parameters: 2
// output_parameters: 2
// instruction_count: 17
// float_instruction_count: 6
// uint_instruction_count: 3
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(131060, 3256);
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
    return std::span(_data).subspan(134316, 4944);
}

// shader_hash: 22751d6713db53a95263c35d46c1dc99
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 2
// output_parameters: 4
// instruction_count: 63
// float_instruction_count: 7
// texture_load_instructions: 3
auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(139260, 5116);
}

// shader_hash: 188e6e7c7b42987d6c520c4af735c117
// constant_buffers: 1
// bound_resources: 1
// input_parameters: 4
// output_parameters: 1
// instruction_count: 58
// dynamic_flow_control_count: 1
// float_instruction_count: 27
// int_instruction_count: 1
// texture_normal_instructions: 1
auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(144376, 4824);
}

} // namespace fb::baked
