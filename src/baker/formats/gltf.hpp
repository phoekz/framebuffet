#pragma once

#include "../pch.hpp"
#include "image.hpp"

namespace fb {

using GltfVertexPosition = Float3;
using GltfVertexNormal = Float3;
using GltfVertexTexcoord = Float2;
using GltfVertexJoint = Uint4;
using GltfVertexWeight = Float4;
using GltfIndex = uint32_t;

struct GltfChannelHeader {
    size_t t_offset = 0;
    size_t t_count = 0;
    size_t r_offset = 0;
    size_t r_count = 0;
    size_t s_offset = 0;
    size_t s_count = 0;
};

inline constexpr uint32_t GLTF_NULL_NODE = ~0u;

// Gltf spec: The base color texture MUST contain 8-bit values encoded with the
// sRGB opto-electronic transfer function so RGB values MUST be decoded to real
// linear values before they are used for any computations.
inline constexpr DXGI_FORMAT GLTF_BASE_COLOR_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
inline constexpr DXGI_FORMAT GLTF_NORMAL_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr DXGI_FORMAT GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

class GltfModel {
public:
    GltfModel(std::string_view gltf_path);

    // clang-format off
    auto vertex_positions() const -> std::span<const GltfVertexPosition> { return _vertex_positions; }
    auto vertex_normals() const -> std::span<const GltfVertexNormal> { return _vertex_normals; }
    auto vertex_texcoords() const -> std::span<const GltfVertexTexcoord> { return _vertex_texcoords; }
    auto vertex_joints() const -> std::span<const GltfVertexJoint> { return _vertex_joints; }
    auto vertex_weights() const -> std::span<const GltfVertexWeight> { return _vertex_weights; }
    auto indices() const -> std::span<const uint32_t> { return _indices; }

    auto base_color_texture() const -> const LdrImage& { return _base_color_texture; }
    auto normal_texture() const -> const std::optional<std::reference_wrapper<const LdrImage>> {
        if (_normal_texture.data().empty()) {
            return std::nullopt;
        }
        return _normal_texture;
    }
    auto metallic_roughness_texture() const -> const std::optional<std::reference_wrapper<const LdrImage>> {
        if (_metallic_roughness_texture.data().empty()) {
            return std::nullopt;
        }
        return _metallic_roughness_texture;
    }

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
    std::vector<GltfVertexPosition> _vertex_positions;
    std::vector<GltfVertexNormal> _vertex_normals;
    std::vector<GltfVertexTexcoord> _vertex_texcoords;
    std::vector<GltfVertexJoint> _vertex_joints;
    std::vector<GltfVertexWeight> _vertex_weights;
    std::vector<uint32_t> _indices;

    LdrImage _base_color_texture;
    LdrImage _normal_texture;
    LdrImage _metallic_roughness_texture;

    float _metallic_factor = 1.0f;
    float _roughness_factor = 1.0f;

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
