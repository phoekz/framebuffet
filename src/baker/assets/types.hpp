#pragma once

#include "../pch.hpp"

namespace fb {

struct AssetSpan {
    std::string type;
    size_t offset;
    size_t element_size;
    size_t byte_size;
};

struct AssetCopy {
    std::string name;
    AssetSpan data;
};

struct AssetVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
};

struct AssetSkinningVertex {
    Float3 position;
    Float3 normal;
    Float2 texcoord;
    Float4 tangent;
    Uint4 joint;
    Float4 weight;
};

using AssetIndex = uint32_t;

struct AssetMesh {
    std::string name;

    AssetSpan vertices;
    AssetSpan indices;
};

struct AssetTextureData {
    uint32_t row_pitch;
    uint32_t slice_pitch;
    AssetSpan data;
};

inline constexpr uint32_t MAX_MIP_COUNT = 12;

struct AssetTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
    uint32_t mip_count;
    std::array<AssetTextureData, MAX_MIP_COUNT> datas;
};

struct AssetCubeTexture {
    std::string name;

    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint32_t channel_count;
    uint32_t row_pitch;
    uint32_t slice_pitch;
    std::array<AssetSpan, 6> datas;
};

struct AssetAnimationMesh {
    std::string name;

    uint32_t node_count;
    uint32_t joint_count;
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
