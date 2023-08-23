#pragma once

#include "../pch.hpp"

namespace fb {

enum class ShaderType {
    Unknown,
    Compute,
    Vertex,
    Pixel,
};

struct ShaderCounters {
    auto to_comment_string() const -> std::string;

    uint32_t constant_buffers = 0;
    uint32_t bound_resources = 0;
    uint32_t input_parameters = 0;
    uint32_t output_parameters = 0;
    uint32_t instruction_count = 0;
    uint32_t temp_array_count = 0;
    uint32_t dynamic_flow_control_count = 0;
    uint32_t array_instruction_count = 0;
    uint32_t float_instruction_count = 0;
    uint32_t int_instruction_count = 0;
    uint32_t uint_instruction_count = 0;
    uint32_t texture_normal_instructions = 0;
    uint32_t texture_load_instructions = 0;
    uint32_t texture_comp_instructions = 0;
    uint32_t texture_bias_instructions = 0;
    uint32_t texture_gradient_instructions = 0;
    uint32_t cut_instruction_count = 0;
    uint32_t emit_instruction_count = 0;
    uint32_t barrier_instructions = 0;
    uint32_t interlocked_instructions = 0;
    uint32_t texture_store_instructions = 0;
};

struct Shader {
    std::string name;
    std::string hash;
    std::vector<std::byte> dxil;
    std::vector<std::byte> pdb;
    ShaderCounters counters;
};

class ShaderCompiler {
public:
    ShaderCompiler();

    auto compile(
        std::string_view name,
        ShaderType type,
        std::string_view entry_point,
        std::span<const std::byte> source,
        bool debug = false
    ) const -> Shader;

private:
    ComPtr<IDxcCompiler3> _compiler;
    ComPtr<IDxcUtils> _utils;
    ComPtr<IDxcIncludeHandler> _include_handler;
};

} // namespace fb

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
