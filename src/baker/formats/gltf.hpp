#pragma once

#include "../pch.hpp"
#include "image.hpp"

namespace fb {

struct GltfVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
};

using GltfIndex = uint32_t;

struct GltfSkinningVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Uint4 joints;
    Float4 weights;
};

struct GltfChannelHeader {
    size_t t_offset = 0;
    size_t t_count = 0;
    size_t r_offset = 0;
    size_t r_count = 0;
    size_t s_offset = 0;
    size_t s_count = 0;
};
inline constexpr uint32_t GLTF_NULL_NODE = ~0u;

class GltfModel {
  public:
    GltfModel(std::string_view gltf_path);

    // clang-format off
    auto vertices() const -> std::span<const GltfVertex> { return _vertices; }
    auto indices() const -> std::span<const uint32_t> { return _indices; }
    auto base_color_texture() const -> const Image& { return _base_color_texture; }
    auto skinning_vertices() const -> std::span<const GltfSkinningVertex> { return _skinning_vertices; };

    auto node_count() const -> uint32_t { return (uint32_t)_node_transforms.size(); }
    auto joint_count() const -> uint32_t { return (uint32_t)_joint_nodes.size(); }
    auto animation_duration() const -> float { return _animation_duration; };

    auto joint_nodes() const -> std::span<const uint32_t> { return _joint_nodes; };
    auto joint_inverse_binds() const -> std::span<const Float4x4> { return _joint_inverse_binds; };
    auto node_parents() const -> std::span<const uint32_t> { return _node_parents; };
    auto node_transforms() const -> std::span<const Float4x4> { return _node_transforms; };
    auto node_channels() const -> std::span<const GltfChannelHeader> { return _node_channels; };
    auto node_channels_times_t() const -> std::span<const float> { return _node_channels_times_t; };
    auto node_channels_times_r() const -> std::span<const float> { return _node_channels_times_r; };
    auto node_channels_times_s() const -> std::span<const float> { return _node_channels_times_s; };
    auto node_channels_values_t() const -> std::span<const Float3> { return _node_channels_values_t; };
    auto node_channels_values_r() const -> std::span<const Quaternion> { return _node_channels_values_r; };
    auto node_channels_values_s() const -> std::span<const Float3> { return _node_channels_values_s; };
    // clang-format on

  private:
    std::vector<GltfVertex> _vertices;
    std::vector<uint32_t> _indices;
    Image _base_color_texture;

    std::vector<GltfSkinningVertex> _skinning_vertices;
    std::vector<uint32_t> _joint_nodes;
    std::vector<Float4x4> _joint_inverse_binds;
    std::vector<uint32_t> _node_parents;
    std::vector<Float4x4> _node_transforms;
    std::vector<GltfChannelHeader> _node_channels;
    std::vector<float> _node_channels_times_t;
    std::vector<float> _node_channels_times_r;
    std::vector<float> _node_channels_times_s;
    std::vector<Float3> _node_channels_values_t;
    std::vector<Quaternion> _node_channels_values_r;
    std::vector<Float3> _node_channels_values_s;
    float _animation_duration = 0.0f;
};

} // namespace fb
