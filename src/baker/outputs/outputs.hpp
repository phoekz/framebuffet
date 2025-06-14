#pragma once

#include "../assets/tasks.hpp"
#include "../assets/types.hpp"
#include "../shaders/shaders.hpp"

namespace fb {

auto bake_app_datas(
    Span<const std::string_view> output_dirs,
    std::string_view app_name,
    Span<const AssetTask> app_asset_tasks,
    Span<const ShaderTask> app_shader_tasks
) -> void;

} // namespace fb
