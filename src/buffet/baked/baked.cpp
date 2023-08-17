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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
        .format = DXGI_FORMAT_R8G8B8A8_UNORM,
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
    FB_ASSERT(_data.size() == 130628);
}

auto Shaders::gui_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(0, 5252);
}

auto Shaders::gui_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(5252, 4400);
}

auto Shaders::debug_draw_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(9652, 5100);
}

auto Shaders::debug_draw_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(14752, 3028);
}

auto Shaders::cards_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(17780, 4836);
}

auto Shaders::cards_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(22616, 4208);
}

auto Shaders::cube_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(26824, 4996);
}

auto Shaders::cube_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(31820, 4556);
}

auto Shaders::tree_shadow_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(36376, 4632);
}

auto Shaders::tree_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(41008, 5432);
}

auto Shaders::tree_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(46440, 5196);
}

auto Shaders::rain_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(51636, 3724);
}

auto Shaders::rain_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(55360, 5040);
}

auto Shaders::rain_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(60400, 3208);
}

auto Shaders::anim_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(63608, 9968);
}

auto Shaders::anim_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(73576, 4636);
}

auto Shaders::fibers_sim_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(78212, 3980);
}

auto Shaders::fibers_reset_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(82192, 3400);
}

auto Shaders::fibers_cull_cs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(85592, 7340);
}

auto Shaders::fibers_light_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(92932, 5464);
}

auto Shaders::fibers_light_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(98396, 3120);
}

auto Shaders::fibers_plane_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(101516, 5480);
}

auto Shaders::fibers_plane_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(106996, 5928);
}

auto Shaders::fibers_debug_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(112924, 3248);
}

auto Shaders::fibers_debug_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(116172, 4932);
}

auto Shaders::env_draw_vs() const -> std::span<const std::byte> {
    return std::span(_data).subspan(121104, 5096);
}

auto Shaders::env_draw_ps() const -> std::span<const std::byte> {
    return std::span(_data).subspan(126200, 4428);
}

}  // namespace fb::baked
