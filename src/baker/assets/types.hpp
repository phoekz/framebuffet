#pragma once

#include "../pch.hpp"

namespace fb {

struct AssetSpan {
    std::string type;
    size_t offset;
    size_t element_count;
    size_t byte_count;
};

struct AssetCopy {
    std::string name;
    AssetSpan data;
};

struct AssetVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
};

struct AssetSkinningVertex {
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 tangent;
    uint4 joint;
    float4 weight;
};

using AssetIndex = uint;

struct AssetMesh {
    std::string name;

    AssetSpan vertices;
    AssetSpan indices;
};

struct AssetTextureData {
    uint row_pitch;
    uint slice_pitch;
    AssetSpan data;
};

inline constexpr uint MAX_MIP_COUNT = 12;

struct AssetTexture {
    std::string name;

    DXGI_FORMAT format;
    uint width;
    uint height;
    uint mip_count;
    std::array<AssetTextureData, MAX_MIP_COUNT> datas;
};

struct AssetCubeTexture {
    std::string name;

    DXGI_FORMAT format;
    uint width;
    uint height;
    uint mip_count;
    std::array<std::array<AssetTextureData, MAX_MIP_COUNT>, 6> datas;
};

struct AssetAnimationMesh {
    std::string name;

    uint node_count;
    uint joint_count;
    float duration;

    AssetSpan skinning_vertices;
    AssetSpan indices;
    AssetSpan joint_nodes;
    AssetSpan joint_inverse_binds;
    AssetSpan node_parents;
    AssetSpan node_transforms;
    AssetSpan node_channels;
    AssetSpan node_channels_times_t;
    AssetSpan node_channels_times_r;
    AssetSpan node_channels_times_s;
    AssetSpan node_channels_values_t;
    AssetSpan node_channels_values_r;
    AssetSpan node_channels_values_s;
};

using Asset =
    std::variant<AssetCopy, AssetMesh, AssetTexture, AssetCubeTexture, AssetAnimationMesh>;

} // namespace fb
