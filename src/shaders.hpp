#pragma once

#include "utils.hpp"
#include <pch.hpp>

namespace fb {

enum class ShaderType {
    Compute,
    Vertex,
    Pixel,
};

class Shader {
    friend class ShaderCompiler;

  public:
    auto blob() const -> IDxcBlob* { return _blob.get(); }
    auto type() const -> ShaderType { return _type; }
    auto bytecode() const -> D3D12_SHADER_BYTECODE;

  private:
    ComPtr<IDxcBlob> _blob;
    ShaderType _type;
};

class ShaderCompiler {
  public:
    ShaderCompiler();

    auto compile(
        std::string_view name,
        ShaderType type,
        std::string_view entry_point,
        std::span<const std::byte> source,
        bool debug = false) -> Shader;

  private:
    ComPtr<IDxcCompiler3> _compiler;
    ComPtr<IDxcUtils> _utils;
    ComPtr<IDxcIncludeHandler> _include_handler;
};

}  // namespace fb
