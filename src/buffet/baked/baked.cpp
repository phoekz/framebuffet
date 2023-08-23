#include "baked.hpp"

namespace fb::baked {

Assets::Assets() {
    _data = read_whole_file("fb_assets.bin");
    FB_ASSERT(_data.size() == 127530132);
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

auto Assets::sci_fi_case_base_color_texture() const -> Texture {
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

auto Assets::sci_fi_case_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(4477344, 4194304),
    };
}

auto Assets::sci_fi_case_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(8671648, 4194304),
    };
}

auto Assets::metal_plane_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(12865952, 4),
        .indices = transmuted_span<Index>(12866144, 6),
    };
}

auto Assets::metal_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(12866168, 4194304),
    };
}

auto Assets::metal_plane_normal_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(17060472, 4194304),
    };
}

auto Assets::metal_plane_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 1024,
        .height = 1024,
        .channels = 4,
        .row_pitch = 4096,
        .slice_pitch = 4194304,
        .data = transmuted_span<std::byte>(21254776, 4194304),
    };
}

auto Assets::coconut_tree_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(25449080, 725),
        .indices = transmuted_span<Index>(25483880, 2034),
    };
}

auto Assets::coconut_tree_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 512,
        .height = 512,
        .channels = 4,
        .row_pitch = 2048,
        .slice_pitch = 1048576,
        .data = transmuted_span<std::byte>(25492016, 1048576),
    };
}

auto Assets::sand_plane_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(26540592, 4),
        .indices = transmuted_span<Index>(26540784, 6),
    };
}

auto Assets::sand_plane_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 64,
        .height = 64,
        .channels = 4,
        .row_pitch = 256,
        .slice_pitch = 16384,
        .data = transmuted_span<std::byte>(26540808, 16384),
    };
}

auto Assets::raccoon_animation_mesh() const -> AnimationMesh {
    return AnimationMesh {
        .node_count = 72,
        .joint_count = 63,
        .duration = 0.7916667f,
        .skinning_vertices = transmuted_span<SkinningVertex>(26557192, 2430),
        .indices = transmuted_span<Index>(26751592, 3102),
        .joint_nodes = transmuted_span<uint32_t>(26764000, 63),
        .joint_inverse_binds = transmuted_span<Float4x4>(26764252, 63),
        .node_parents = transmuted_span<uint32_t>(26768284, 72),
        .node_transforms = transmuted_span<Float4x4>(26768572, 72),
        .node_channels = transmuted_span<AnimationChannel>(26773180, 72),
        .node_channels_times_t = transmuted_span<float>(26776636, 2256),
        .node_channels_times_r = transmuted_span<float>(26785660, 2274),
        .node_channels_times_s = transmuted_span<float>(26794756, 369),
        .node_channels_values_t = transmuted_span<Float3>(26796232, 2256),
        .node_channels_values_r = transmuted_span<Quaternion>(26823304, 2274),
        .node_channels_values_s = transmuted_span<Float3>(26859688, 369),
    };
}

auto Assets::raccoon_base_color_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .width = 4,
        .height = 4,
        .channels = 4,
        .row_pitch = 16,
        .slice_pitch = 64,
        .data = transmuted_span<std::byte>(26864116, 64),
    };
}

auto Assets::raccoon_metallic_roughness_texture() const -> Texture {
    return Texture {
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .width = 4,
        .height = 4,
        .channels = 4,
        .row_pitch = 16,
        .slice_pitch = 64,
        .data = transmuted_span<std::byte>(26864180, 64),
    };
}

auto Assets::light_bounds_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(26864244, 24),
        .indices = transmuted_span<Index>(26865396, 36),
    };
}

auto Assets::skybox_mesh() const -> Mesh {
    return Mesh {
        .vertices = transmuted_span<Vertex>(26865540, 24),
        .indices = transmuted_span<Index>(26866692, 36),
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
            transmuted_span<float>(26866836, 4194304),
            transmuted_span<float>(43644052, 4194304),
            transmuted_span<float>(60421268, 4194304),
            transmuted_span<float>(77198484, 4194304),
            transmuted_span<float>(93975700, 4194304),
            transmuted_span<float>(110752916, 4194304),
        }),
    };
}

Shaders::Shaders() {
    _data = read_whole_file("fb_shaders.bin");
    FB_ASSERT(_data.size() == 147328);
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

// 47aeb2be7e36a430ebf09731c029cbf2
auto Shaders::crate_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(40888, 5524);
}

// 3ba2216aac1d6ff4d4a37fdbf7ee4ec3
auto Shaders::crate_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46412, 6004);
}

// bb318ca5796644e7a6c68671f7011d41
auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(52416, 4652);
}

// 4f92468768d022e91d050a21946f749f
auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(57068, 5456);
}

// d8fc468d518d682e1b21e1879a32d5e3
auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(62524, 5216);
}

// d7bde9ff419de59f4788f766651525ae
auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(67740, 3748);
}

// 0820cea972058c552f588fd8ea7b324e
auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(71488, 5064);
}

// 785867d3fd40ec288756abc5b7fa1a8a
auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(76552, 3228);
}

// 375a5653c5bb68d44e67412aa73d0ae3
auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(79780, 10188);
}

// 3451eb25b73e7f9e7038c3afaf0e67a6
auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(89968, 4776);
}

// 38367bd0b1752e4ca0df66b75c2c3e42
auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(94744, 4004);
}

// 27a8754335d1b1087f852a95afe6cf98
auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(98748, 3424);
}

// 6948df21d6f52241ffd77c4cc5ae1817
auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(102172, 7328);
}

// 00bdae7a717783ec01c1587d7fe7f16b
auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(109500, 5488);
}

// 0d545b607d54aea303eb4a8db4cf31da
auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(114988, 3144);
}

// 67ce76c1b8cef37afea87f3af4e63980
auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(118132, 5508);
}

// bf043e6ca1fc244f4228b30f10d7a497
auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(123640, 5928);
}

// 774cf19def534ea19fa2c04206753609
auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(129568, 3256);
}

// fba11f278d24921065a7ff897de265ce
auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(132824, 4944);
}

// 22751d6713db53a95263c35d46c1dc99
auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(137768, 5116);
}

// 34759f4385b2a6b7410f8378fc3e6341
auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(142884, 4444);
}

} // namespace fb::baked
