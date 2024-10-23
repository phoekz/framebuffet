#pragma once

#include <common/common.hpp>

namespace fb::baked {

struct Copy {
    std::span<const std::byte> data;
};

struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
};

struct SkinningVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
    uint4 joints;
    float4 weights;
};

using Index = uint;

struct Submesh {
    uint index_count;
    uint start_index;
    uint base_vertex;
};

struct Mesh {
    float4x4 transform;
    std::span<const Vertex> vertices;
    std::span<const Index> indices;
    std::span<const Submesh> submeshes;
};

inline constexpr uint MAX_MIP_COUNT = 12;

struct TextureData {
    uint row_pitch;
    uint slice_pitch;
    std::span<const std::byte> data;
};

struct Texture {
    DXGI_FORMAT format;
    uint width;
    uint height;
    uint channel_count;
    uint mip_count;
    std::array<TextureData, MAX_MIP_COUNT> datas;
};

enum class CubeFace : uint {
    PosX,
    NegX,
    PosY,
    NegY,
    PosZ,
    NegZ,
};

struct CubeTexture {
    DXGI_FORMAT format;
    uint width;
    uint height;
    uint channel_count;
    uint mip_count;
    std::array<std::array<TextureData, MAX_MIP_COUNT>, 6> datas;
};

enum class AlphaMode : uint {
    Opaque,
    Mask,
};

struct Material {
    float alpha_cutoff;
    AlphaMode alpha_mode;
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
    float4x4 transform;
    uint node_count;
    uint joint_count;
    float duration;
    std::span<const SkinningVertex> skinning_vertices;
    std::span<const Index> indices;
    std::span<const Submesh> submeshes;
    std::span<const uint> joint_nodes;
    std::span<const float4x4> joint_inverse_binds;
    std::span<const uint> node_parents;
    std::span<const AnimationChannel> node_channels;
    std::span<const float> node_channels_times_t;
    std::span<const float> node_channels_times_r;
    std::span<const float> node_channels_times_s;
    std::span<const float3> node_channels_values_t;
    std::span<const float_quat> node_channels_values_r;
    std::span<const float3> node_channels_values_s;
};

struct Glyph {
    uint character;
    float2 xbounds;
    float2 ybounds;
    float advance;
    float lbearing;
    float rbearing;
};

struct Font {
    float ascender;
    float descender;
    float space_advance;
    std::span<const Glyph> glyphs;
};

} // namespace fb::baked
