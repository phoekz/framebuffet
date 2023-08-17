#pragma once

#include "../pch.hpp"

namespace fb {

enum class ShaderType {
    Unknown,
    Compute,
    Vertex,
    Pixel,
};

struct Shader {
    std::string name;
    std::string hash;
    std::vector<std::byte> dxil;
    std::vector<std::byte> pdb;
};

class ShaderCompiler {
  public:
    ShaderCompiler();

    auto compile(
        std::string_view name,
        ShaderType type,
        std::string_view entry_point,
        std::span<const std::byte> source,
        bool debug = false) const -> Shader;

  private:
    ComPtr<IDxcCompiler3> _compiler;
    ComPtr<IDxcUtils> _utils;
    ComPtr<IDxcIncludeHandler> _include_handler;
};

}  // namespace fb

template<>
struct std::formatter<fb::ShaderType>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::ShaderType v, FormatContext& fc) const {
        switch (v) {
            case fb::ShaderType::Compute: return std::format_to(fc.out(), "Compute");
            case fb::ShaderType::Vertex: return std::format_to(fc.out(), "Vertex");
            case fb::ShaderType::Pixel: return std::format_to(fc.out(), "Pixel");
            default: return std::format_to(fc.out(), "Unknown");
        }
    }
};
