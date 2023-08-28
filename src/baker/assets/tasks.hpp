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

struct AssetTaskCubeTexture {
    std::string_view name;
    std::array<std::string_view, 6> paths;
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

using AssetTask = std::variant<
    AssetTaskCopy,
    AssetTaskTexture,
    AssetTaskHdrTexture,
    AssetTaskCubeTexture,
    AssetTaskGltf,
    AssetTaskProceduralCube,
    AssetTaskProceduralSphere>;

inline constexpr auto asset_task_name(size_t variant) -> std::string_view {
    switch (variant) {
        case 0: return "AssetTaskCopy";
        case 1: return "AssetTaskTexture";
        case 2: return "AssetTaskHdrTexture";
        case 3: return "AssetTaskCubeTexture";
        case 4: return "AssetTaskGltf";
        case 5: return "AssetTaskProceduralCube";
        case 6: return "AssetTaskProceduralSphere";
        default: FB_FATAL();
    }
}

auto bake_assets(std::string_view assets_dir, std::span<const AssetTask> asset_tasks)
    -> std::tuple<std::vector<Asset>, std::vector<std::byte>>;

} // namespace fb
