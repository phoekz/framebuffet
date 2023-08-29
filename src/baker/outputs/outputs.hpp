#pragma once

#include "../pch.hpp"
#include "../assets/tasks.hpp"
#include "../assets/types.hpp"
#include "../shaders/shaders.hpp"

namespace fb {

auto bake_app_datas(
    std::span<const std::string_view> output_dirs,
    std::string_view app_name,
    std::span<const AssetTask> app_asset_tasks,
    std::span<const ShaderTask> app_shader_tasks
) -> void;

} // namespace fb
