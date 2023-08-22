#include "baked.hpp"

namespace fb::baked {

Assets::Assets() {
    _data = read_whole_file("fb_assets.bin");
    FB_ASSERT(_data.size() == 106558332);
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

auto Assets::sci_fi_case_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(164636, 2025),
        .indices = transmuted_span<Index>(261836, 5301),
    };
}

auto Assets::sci_fi_case_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(283040, 4194304),
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(4477344, 725),
        .indices = transmuted_span<Index>(4512144, 2034),
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
        .data = transmuted_span<std::byte>(4520280, 1048576),
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5568856, 4),
        .indices = transmuted_span<Index>(5569048, 6),
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
        .data = transmuted_span<std::byte>(5569072, 16384),
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        .skinning_vertices = transmuted_span<SkinningVertex>(5585456, 2430),
        .indices = transmuted_span<Index>(5779856, 3102),
        .joint_nodes = transmuted_span<uint32_t>(5792264, 63),
        .joint_inverse_binds = transmuted_span<Float4x4>(5792516, 63),
        .node_parents = transmuted_span<uint32_t>(5796548, 72),
        .node_transforms = transmuted_span<Float4x4>(5796836, 72),
        .node_channels = transmuted_span<AnimationChannel>(5801444, 72),
        .node_channels_times_t = transmuted_span<float>(5804900, 2256),
        .node_channels_times_r = transmuted_span<float>(5813924, 2274),
        .node_channels_times_s = transmuted_span<float>(5823020, 369),
        .node_channels_values_t = transmuted_span<Float3>(5824496, 2256),
        .node_channels_values_r = transmuted_span<Quaternion>(5851568, 2274),
        .node_channels_values_s = transmuted_span<Float3>(5887952, 369),
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
        .data = transmuted_span<std::byte>(5892380, 64),
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5892444, 24),
        .indices = transmuted_span<Index>(5893596, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(5893740, 24),
        .indices = transmuted_span<Index>(5894892, 36),
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
            transmuted_span<float>(5895036, 4194304),
            transmuted_span<float>(22672252, 4194304),
            transmuted_span<float>(39449468, 4194304),
            transmuted_span<float>(56226684, 4194304),
            transmuted_span<float>(73003900, 4194304),
            transmuted_span<float>(89781116, 4194304),
        }),
    };
}

Shaders::Shaders() {
    _data = read_whole_file("fb_shaders.bin");
    FB_ASSERT(_data.size() == 146568);
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

// 3392d089ff06c589a929ab0793db2127
auto Shaders::cube_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(40888, 5424);
}

// 70f39ffdcd11787931dfe7ff3ecef6e0
auto Shaders::cube_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46312, 5344);
}

// bb318ca5796644e7a6c68671f7011d41
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(51656, 4652);
}

// 4f92468768d022e91d050a21946f749f
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(56308, 5456);
}

// d8fc468d518d682e1b21e1879a32d5e3
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(61764, 5216);
}

// d7bde9ff419de59f4788f766651525ae
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(66980, 3748);
}

// 0820cea972058c552f588fd8ea7b324e
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(70728, 5064);
}

// 785867d3fd40ec288756abc5b7fa1a8a
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(75792, 3228);
}

// 375a5653c5bb68d44e67412aa73d0ae3
auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(79020, 10188);
}

// 3451eb25b73e7f9e7038c3afaf0e67a6
auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(89208, 4776);
}

// 38367bd0b1752e4ca0df66b75c2c3e42
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(93984, 4004);
}

// 27a8754335d1b1087f852a95afe6cf98
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(97988, 3424);
}

// 6948df21d6f52241ffd77c4cc5ae1817
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(101412, 7328);
}

// 00bdae7a717783ec01c1587d7fe7f16b
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(108740, 5488);
}

// 0d545b607d54aea303eb4a8db4cf31da
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(114228, 3144);
}

// 67ce76c1b8cef37afea87f3af4e63980
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(117372, 5508);
}

// bf043e6ca1fc244f4228b30f10d7a497
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(122880, 5928);
}

// 774cf19def534ea19fa2c04206753609
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(128808, 3256);
}

// fba11f278d24921065a7ff897de265ce
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(132064, 4944);
}

// 22751d6713db53a95263c35d46c1dc99
auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(137008, 5116);
}

// 34759f4385b2a6b7410f8378fc3e6341
auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(142124, 4444);
}

} // namespace fb::baked
