#pragma once

#include "pch.hpp"

namespace fb::baked {

struct Copy {
    std::span<const std::byte> data;
};

struct Vertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
};

struct SkinningVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
    Uint4 joints;
    Float4 weights;
};

using Index = uint32_t;

struct Mesh {
    std::span<const Vertex> vertices;
    std::span<const Index> indices;
};

inline constexpr uint32_t MAX_MIP_COUNT = 12;

struct TextureData {
    uint32_t row_pitch;
    uint32_t slice_pitch;
    std::span<const std::byte> data;
};

struct Texture {
    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
    uint32_t mip_count;
    std::array<TextureData, MAX_MIP_COUNT> datas;
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
    uint32_t mip_count;
    std::array<std::array<TextureData, MAX_MIP_COUNT>, 6> datas;
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

} // namespace fb::baked
