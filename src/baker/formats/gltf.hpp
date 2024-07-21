#pragma once

#include "../pch.hpp"
#include "image.hpp"

namespace fb {

using GltfVertexPosition = float3;
using GltfVertexNormal = float3;
using GltfVertexTexcoord = float2;
using GltfVertexJoint = uint4;
using GltfVertexWeight = float4;
using GltfIndex = uint;

struct GltfSubmesh {
    uint index_count;
    uint start_index;
};

struct GltfChannelHeader {
    size_t t_offset = 0;
    size_t t_count = 0;
    size_t r_offset = 0;
    size_t r_count = 0;
    size_t s_offset = 0;
    size_t s_count = 0;
};

inline constexpr uint GLTF_NULL_NODE = ~0u;

// Gltf spec: The base color texture MUST contain 8-bit values encoded with the
// sRGB opto-electronic transfer function so RGB values MUST be decoded to real
// linear values before they are used for any computations.
inline constexpr DXGI_FORMAT GLTF_BASE_COLOR_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
inline constexpr DXGI_FORMAT GLTF_NORMAL_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr DXGI_FORMAT GLTF_METALLIC_ROUGHNESS_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

enum class GltfAlphaMode : uint {
    Opaque = 0,
    Mask,
};

class GltfModel {
public:
    GltfModel(std::string_view gltf_path);

    auto root_transform() const -> float4x4 { return _root_transform; }

    // clang-format off
    auto vertex_positions() const -> std::span<const GltfVertexPosition> { return _vertex_positions; }
    auto vertex_normals() const -> std::span<const GltfVertexNormal> { return _vertex_normals; }
    auto vertex_texcoords() const -> std::span<const GltfVertexTexcoord> { return _vertex_texcoords; }
    auto vertex_joints() const -> std::span<const GltfVertexJoint> { return _vertex_joints; }
    auto vertex_weights() const -> std::span<const GltfVertexWeight> { return _vertex_weights; }
    auto indices() const -> std::span<const uint> { return _indices; }
    auto submeshes() const -> std::span<const GltfSubmesh> { return _submeshes; }

    auto base_color_texture() const -> const LdrImage& { return _base_color_texture; }
    auto normal_texture() const -> const Option<ConstRef<LdrImage>> {
        if (_normal_texture.data().empty()) {
            return std::nullopt;
        }
        return _normal_texture;
    }
    auto metallic_roughness_texture() const -> const Option<ConstRef<LdrImage>> {
        if (_metallic_roughness_texture.data().empty()) {
            return std::nullopt;
        }
        return _metallic_roughness_texture;
    }
    auto alpha_cutoff() const -> float { return _alpha_cutoff; }
    auto alpha_mode() const -> GltfAlphaMode { return _alpha_mode; }

    auto node_count() const -> uint { return (uint)_node_channels.size(); }
    auto joint_count() const -> uint { return (uint)_joint_nodes.size(); }
    auto animation_duration() const -> float { return _animation_duration; };

    auto joint_nodes() const -> std::span<const uint> { return _joint_nodes; };
    auto joint_inverse_binds() const -> std::span<const float4x4> { return _joint_inverse_binds; };
    auto node_parents() const -> std::span<const uint> { return _node_parents; };
    auto node_channels() const -> std::span<const GltfChannelHeader> { return _node_channels; };
    auto node_channels_times_t() const -> std::span<const float> { return _node_channels_times_t; };
    auto node_channels_times_r() const -> std::span<const float> { return _node_channels_times_r; };
    auto node_channels_times_s() const -> std::span<const float> { return _node_channels_times_s; };
    auto node_channels_values_t() const -> std::span<const float3> { return _node_channels_values_t; };
    auto node_channels_values_r() const -> std::span<const Quaternion> { return _node_channels_values_r; };
    auto node_channels_values_s() const -> std::span<const float3> { return _node_channels_values_s; };
    // clang-format on

private:
    float4x4 _root_transform = float4x4::Identity;

    std::vector<GltfVertexPosition> _vertex_positions;
    std::vector<GltfVertexNormal> _vertex_normals;
    std::vector<GltfVertexTexcoord> _vertex_texcoords;
    std::vector<GltfVertexJoint> _vertex_joints;
    std::vector<GltfVertexWeight> _vertex_weights;
    std::vector<uint> _indices;
    std::vector<GltfSubmesh> _submeshes;

    LdrImage _base_color_texture;
    LdrImage _normal_texture;
    LdrImage _metallic_roughness_texture;

    float _metallic_factor = 1.0f;
    float _roughness_factor = 1.0f;
    float _alpha_cutoff = 0.0f;
    GltfAlphaMode _alpha_mode = GltfAlphaMode::Opaque;

    std::vector<uint> _joint_nodes;
    std::vector<float4x4> _joint_inverse_binds;
    std::vector<uint> _node_parents;
    std::vector<GltfChannelHeader> _node_channels;
    std::vector<float> _node_channels_times_t;
    std::vector<float> _node_channels_times_r;
    std::vector<float> _node_channels_times_s;
    std::vector<float3> _node_channels_values_t;
    std::vector<Quaternion> _node_channels_values_r;
    std::vector<float3> _node_channels_values_s;
    float _animation_duration = 0.0f;
};

} // namespace fb
