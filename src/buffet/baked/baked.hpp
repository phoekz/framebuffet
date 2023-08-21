#pragma once

#include "../pch.hpp"

namespace fb::baked {

struct Copy {
    std::span<const std::byte> data;
};

struct Vertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
};

struct SkinningVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Uint4 joints;
    Float4 weights;
};

using Index = uint32_t;

struct Mesh {
    std::span<const Vertex> vertices;
    std::span<const Index> indices;
};

struct Texture {
    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    std::span<const std::byte> data;
};

enum class CubeFace : uint32_t {
    PosX,
    NegX,
    PosY,
    NegY,
    PosZ,
    NegZ,
};

struct CubeTexture {
    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    std::array<std::span<const float>, 6> datas;
};

struct AnimationChannel {
    size_t t_offset;
    size_t t_count;
    size_t r_offset;
    size_t r_count;
    size_t s_offset;
    size_t s_count;
};

struct AnimationMesh {
    uint32_t node_count;
    uint32_t joint_count;
    float duration;
    std::span<const SkinningVertex> skinning_vertices;
    std::span<const Index> indices;
    std::span<const uint32_t> joint_nodes;
    std::span<const Float4x4> joint_inverse_binds;
    std::span<const uint32_t> node_parents;
    std::span<const Float4x4> node_transforms;
    std::span<const AnimationChannel> node_channels;
    std::span<const float> node_channels_times_t;
    std::span<const float> node_channels_times_r;
    std::span<const float> node_channels_times_s;
    std::span<const Float3> node_channels_values_t;
    std::span<const Quaternion> node_channels_values_r;
    std::span<const Float3> node_channels_values_s;
};

class Assets {
public:
    Assets();

    auto imgui_font() const -> Copy;
    auto heatmap_magma_texture() const -> Texture;
    auto heatmap_viridis_texture() const -> Texture;
    auto stylized_crate_mesh() const -> Mesh;
    auto stylized_crate_texture() const -> Texture;
    auto coconut_tree_mesh() const -> Mesh;
    auto coconut_tree_texture() const -> Texture;
    auto sand_plane_mesh() const -> Mesh;
    auto sand_plane_texture() const -> Texture;
    auto raccoon_animation_mesh() const -> AnimationMesh;
    auto raccoon_texture() const -> Texture;
    auto light_bounds_mesh() const -> Mesh;
    auto skybox_mesh() const -> Mesh;
    auto winter_evening_cube_texture() const -> CubeTexture;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_size) const -> std::span<const T> {
        return std::span((const T*)(_data.data() + offset), element_size);
    }

    std::vector<std::byte> _data;
};

class Shaders {
public:
    Shaders();

    auto gui_draw_vs() const -> std::span<const std::byte>;
    auto gui_draw_ps() const -> std::span<const std::byte>;
    auto debug_draw_draw_vs() const -> std::span<const std::byte>;
    auto debug_draw_draw_ps() const -> std::span<const std::byte>;
    auto cards_draw_vs() const -> std::span<const std::byte>;
    auto cards_draw_ps() const -> std::span<const std::byte>;
    auto spd_downsample_cs() const -> std::span<const std::byte>;
    auto cube_draw_vs() const -> std::span<const std::byte>;
    auto cube_draw_ps() const -> std::span<const std::byte>;
    auto tree_shadow_vs() const -> std::span<const std::byte>;
    auto tree_draw_vs() const -> std::span<const std::byte>;
    auto tree_draw_ps() const -> std::span<const std::byte>;
    auto rain_sim_cs() const -> std::span<const std::byte>;
    auto rain_draw_vs() const -> std::span<const std::byte>;
    auto rain_draw_ps() const -> std::span<const std::byte>;
    auto anim_draw_vs() const -> std::span<const std::byte>;
    auto anim_draw_ps() const -> std::span<const std::byte>;
    auto fibers_sim_cs() const -> std::span<const std::byte>;
    auto fibers_reset_cs() const -> std::span<const std::byte>;
    auto fibers_cull_cs() const -> std::span<const std::byte>;
    auto fibers_light_vs() const -> std::span<const std::byte>;
    auto fibers_light_ps() const -> std::span<const std::byte>;
    auto fibers_plane_vs() const -> std::span<const std::byte>;
    auto fibers_plane_ps() const -> std::span<const std::byte>;
    auto fibers_debug_vs() const -> std::span<const std::byte>;
    auto fibers_debug_ps() const -> std::span<const std::byte>;
    auto env_draw_vs() const -> std::span<const std::byte>;
    auto env_draw_ps() const -> std::span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked
