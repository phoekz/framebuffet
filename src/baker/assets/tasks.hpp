#pragma once

#include "../pch.hpp"

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
    size_t tesselation;
    bool inverted;
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

using AssetTask = std::variant<
    AssetTaskCopy,
    AssetTaskTexture,
    AssetTaskHdrTexture,
    AssetTaskGltf,
    AssetTaskProceduralCube,
    AssetTaskProceduralSphere,
    AssetTaskStockcubeOutput,
    AssetTaskTtf>;

inline constexpr auto asset_task_name(size_t variant) -> std::string_view {
    switch (variant) {
        case 0: return "AssetTaskCopy";
        case 1: return "AssetTaskTexture";
        case 2: return "AssetTaskHdrTexture";
        case 3: return "AssetTaskGltf";
        case 4: return "AssetTaskProceduralCube";
        case 5: return "AssetTaskProceduralSphere";
        case 6: return "AssetTaskStockcubeOutput";
        case 7: return "AssetTaskTtf";
        default: FB_FATAL();
    }
}

auto bake_assets(std::string_view assets_dir, std::span<const AssetTask> asset_tasks)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>>;

} // namespace fb
