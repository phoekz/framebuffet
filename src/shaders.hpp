#pragma once

#include "utils.hpp"
#include <d3dx12/d3dx12.h>
#include <dxcapi.h>
#include <span>
#include <string_view>

namespace fb {

struct Shader {
    D3D12_SHADER_BYTECODE bytecode();
    void release();

    ComPtr<IDxcBlob> blob;
};

enum class ShaderType {
    Compute,
    Vertex,
    Pixel,
};

struct ShaderCompiler {
    ShaderCompiler();
    Shader compile(
        std::string_view name,
        ShaderType type,
        std::string_view entry_point,
        std::span<std::byte> source);

    ComPtr<IDxcCompiler3> compiler;
    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcIncludeHandler> include_handler;
};

}  // namespace fb
