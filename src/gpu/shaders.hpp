#pragma once

#include "utils.hpp"
#include <pch.hpp>

namespace fb {

enum class GpuShaderType {
    Compute,
    Vertex,
    Pixel,
};

class GpuShader {
    friend class GpuShaderCompiler;

  public:
    auto blob() const -> IDxcBlob* { return _blob.get(); }
    auto type() const -> GpuShaderType { return _type; }
    auto bytecode() const -> D3D12_SHADER_BYTECODE;

  private:
    ComPtr<IDxcBlob> _blob;
    GpuShaderType _type;
};

class GpuShaderCompiler {
  public:
    GpuShaderCompiler();

    auto compile(
        std::string_view name,
        GpuShaderType type,
        std::string_view entry_point,
        std::span<const std::byte> source,
        bool debug = false) -> GpuShader;

  private:
    ComPtr<IDxcCompiler3> _compiler;
    ComPtr<IDxcUtils> _utils;
    ComPtr<IDxcIncludeHandler> _include_handler;
};

}  // namespace fb
