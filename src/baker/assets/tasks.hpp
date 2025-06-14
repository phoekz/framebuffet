#pragma once

#include "types.hpp"

namespace fb {

enum class AssetColorSpace {
    Srgb,
    Linear,
};

struct AssetTaskCopy {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskTexture {
    std::string_view name;
    std::string_view path;
    DXGI_FORMAT format;
    AssetColorSpace color_space;
};

struct AssetTaskHdrTexture {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskGltf {
    std::string_view name;
    std::string_view path;
};

struct AssetTaskProceduralCube {
    std::string_view name;
    float extents;
    bool inverted;
};

struct AssetTaskProceduralSphere {
    std::string_view name;
    float radius;
    uint tesselation;
    bool inverted;
};

struct AssetTaskProceduralLowPolyGround {
    std::string_view name;
    uint vertex_count_x;
    uint vertex_count_y;
    float side_length;
    float height_variation;
};

struct AssetTaskProceduralTexturedPlane {
    std::string_view name;
    uint texture_resolution;
    float side_length;
    RgbaFloat color_a;
    RgbaFloat color_b;
};

struct AssetTaskStockcubeOutput {
    std::string_view name;
    std::string_view bin_path;
    std::string_view json_path;
};

struct AssetTaskTtf {
    std::string_view name;
    std::string_view path;
    float depth = 1.0f / 8.0f;
};

// clang-format off
#define ASSET_TASK_LIST(x) \
    x(Copy) \
    x(Texture) \
    x(HdrTexture) \
    x(Gltf) \
    x(ProceduralCube) \
    x(ProceduralSphere) \
    x(ProceduralLowPolyGround) \
    x(ProceduralTexturedPlane) \
    x(StockcubeOutput) \
    x(Ttf)
// clang-format on

#define ASSET_TASK_VARIANT_DEFINE(name) AssetTask##name,
#define ASSET_TASK_TYPE_ENUM_DEFINE(name) name,
#define ASSET_TASK_NAME_CASE_DEFINE(name) \
    case AssetTaskType::name: return "AssetTask" #name;

struct AssetTaskNull {};
using AssetTask = std::variant<ASSET_TASK_LIST(ASSET_TASK_VARIANT_DEFINE) AssetTaskNull>;

enum class AssetTaskType {
    ASSET_TASK_LIST(ASSET_TASK_TYPE_ENUM_DEFINE)
};

FB_INLINE constexpr auto asset_task_name(size_t variant) -> std::string_view {
    switch ((AssetTaskType)variant) {
        ASSET_TASK_LIST(ASSET_TASK_NAME_CASE_DEFINE)
        default: FB_FATAL();
    }
}

auto bake_assets(std::string_view assets_dir, Span<const AssetTask> asset_tasks)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>>;

} // namespace fb
