#include "baked.hpp"

namespace fb::baked {

Assets::Assets() {
    _data = read_whole_file("fb_assets.bin");
    FB_ASSERT(_data.size() == 106392216);
}

auto Assets::imgui_font() const -> Copy {
    return Copy {
        .data = transmuted_span<std::byte>(0, 162588),
    };
}

auto Assets::heatmap_magma_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channels = 4,
        .row_pitch = 1024,
        .slice_pitch = 1024,
        .data = transmuted_span<std::byte>(162588, 1024),
    };
}

auto Assets::heatmap_viridis_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 256,
        .height = 1,
        .channels = 4,
        .row_pitch = 1024,
        .slice_pitch = 1024,
        .data = transmuted_span<std::byte>(163612, 1024),
    };
}

auto Assets::stylized_crate_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(164636, 72),
        .indices = transmuted_span<Index>(166940, 324),
    };
}

auto Assets::stylized_crate_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(168236, 4194304),
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(4362540, 725),
        .indices = transmuted_span<Index>(4385740, 2034),
    };
}

auto Assets::coconut_tree_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .channels = 4,
        .row_pitch = 2048,
        .slice_pitch = 1048576,
        .data = transmuted_span<std::byte>(4393876, 1048576),
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5442452, 4),
        .indices = transmuted_span<Index>(5442580, 6),
    };
}

auto Assets::sand_plane_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .channels = 4,
        .row_pitch = 256,
        .slice_pitch = 16384,
        .data = transmuted_span<std::byte>(5442604, 16384),
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        .skinning_vertices = transmuted_span<SkinningVertex>(5458988, 2430),
        .indices = transmuted_span<Index>(5614508, 3102),
        .joint_nodes = transmuted_span<uint32_t>(5626916, 63),
        .joint_inverse_binds = transmuted_span<Float4x4>(5627168, 63),
        .node_parents = transmuted_span<uint32_t>(5631200, 72),
        .node_transforms = transmuted_span<Float4x4>(5631488, 72),
        .node_channels = transmuted_span<AnimationChannel>(5636096, 72),
        .node_channels_times_t = transmuted_span<float>(5639552, 2256),
        .node_channels_times_r = transmuted_span<float>(5648576, 2274),
        .node_channels_times_s = transmuted_span<float>(5657672, 369),
        .node_channels_values_t = transmuted_span<Float3>(5659148, 2256),
        .node_channels_values_r = transmuted_span<Quaternion>(5686220, 2274),
        .node_channels_values_s = transmuted_span<Float3>(5722604, 369),
    };
}

auto Assets::raccoon_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 4,
        .height = 4,
        .channels = 4,
        .row_pitch = 16,
        .slice_pitch = 64,
        .data = transmuted_span<std::byte>(5727032, 64),
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5727096, 24),
        .indices = transmuted_span<Index>(5727864, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5728008, 24),
        .indices = transmuted_span<Index>(5728776, 36),
    };
}

auto Assets::winter_evening_cube_texture() const -> CubeTexture {
    return CubeTexture {
        .format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 16384,
        .slice_pitch = 16777216,
        .datas = std::to_array({
            transmuted_span<float>(5728920, 4194304),
            transmuted_span<float>(22506136, 4194304),
            transmuted_span<float>(39283352, 4194304),
            transmuted_span<float>(56060568, 4194304),
            transmuted_span<float>(72837784, 4194304),
            transmuted_span<float>(89615000, 4194304),
        }),
    };
}

Shaders::Shaders() {
    _data = read_whole_file("fb_shaders.bin");
    FB_ASSERT(_data.size() == 145056);
}

// a2b037e929fa47e82668128ef69b5d3c
auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5352);
}

// 78391642d962dafe09fcb897ff0baef4
auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5352, 4416);
}

// 90b108a0f53edc3edf0fdbdf347b5003
auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9768, 5120);
}

// 28a5f694ad5b0a73913b0a924d402ba0
auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14888, 3056);
}

// 5792fbf56668c072f9917d0d52e90c6d
auto Shaders::cards_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(17944, 4860);
}

// 0277dbed47bb3692b278d7e759af728f
auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(22804, 4220);
}

// 6529e616c34b9ecea94c7de26fa74cbd
auto Shaders::spd_downsample_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(27024, 13864);
}

// eb171dcf4f4765588cbae395cc5f4f1b
auto Shaders::cube_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(40888, 5012);
}

// e13d6d1479c22aeab4a2a5829bf6fcb5
auto Shaders::cube_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(45900, 4580);
}

// 52d81722531e4e31a574f5108c7dc8cc
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(50480, 4652);
}

// b16cdb28805a6c6c6f2a881598c33d0f
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(55132, 5456);
}

// d8fc468d518d682e1b21e1879a32d5e3
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(60588, 5216);
}

// d7bde9ff419de59f4788f766651525ae
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(65804, 3748);
}

// 0820cea972058c552f588fd8ea7b324e
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(69552, 5064);
}

// 785867d3fd40ec288756abc5b7fa1a8a
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(74616, 3228);
}

// ca3a84f0a01636d33e0c4153728e0809
auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(77844, 9980);
}

// fe2bea0daccf11ac7a446e39cfcfca3c
auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(87824, 4656);
}

// 38367bd0b1752e4ca0df66b75c2c3e42
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(92480, 4004);
}

// 27a8754335d1b1087f852a95afe6cf98
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(96484, 3424);
}

// 6948df21d6f52241ffd77c4cc5ae1817
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(99908, 7328);
}

// 8a903181a4162e088ae222bb818e2c0a
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(107236, 5484);
}

// 0d545b607d54aea303eb4a8db4cf31da
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(112720, 3144);
}

// d3df05d793876fd814042ac2ee4c7520
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(115864, 5504);
}

// bf043e6ca1fc244f4228b30f10d7a497
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(121368, 5928);
}

// 774cf19def534ea19fa2c04206753609
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(127296, 3256);
}

// fba11f278d24921065a7ff897de265ce
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(130552, 4944);
}

// a41014911328d2204ae9cccf7210a09a
auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(135496, 5116);
}

// 34759f4385b2a6b7410f8378fc3e6341
auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(140612, 4444);
}

} // namespace fb::baked
