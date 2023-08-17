#pragma once

#include "utils.hpp"
#include <pch.hpp>

namespace fb {

enum class GpuShaderType {
    Unknown,
    Compute,
    Vertex,
    Pixel,
};

using GpuShaderBytecode = std::vector<std::byte>;

class GpuShaderCompiler {
  public:
    GpuShaderCompiler();

    auto compile(
        std::string_view name,
        GpuShaderType type,
        std::string_view entry_point,
        std::span<const std::byte> source,
        bool debug = false) -> GpuShaderBytecode;

  private:
    ComPtr<IDxcCompiler3> _compiler;
    ComPtr<IDxcUtils> _utils;
    ComPtr<IDxcIncludeHandler> _include_handler;
};

}  // namespace fb
