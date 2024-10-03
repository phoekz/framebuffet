#pragma once

#include <common/common.hpp>

#include <dxcapi.h>

namespace fb {

enum class ShaderType {
    Unknown,
    Compute,
    Vertex,
    Pixel,
};

struct ShaderCounters {
    auto to_comment_string() const -> std::string;

    uint constant_buffers = 0;
    uint bound_resources = 0;
    uint input_parameters = 0;
    uint output_parameters = 0;
    uint instruction_count = 0;
    uint temp_array_count = 0;
    uint dynamic_flow_control_count = 0;
    uint array_instruction_count = 0;
    uint float_instruction_count = 0;
    uint int_instruction_count = 0;
    uint uint_instruction_count = 0;
    uint texture_normal_instructions = 0;
    uint texture_load_instructions = 0;
    uint texture_comp_instructions = 0;
    uint texture_bias_instructions = 0;
    uint texture_gradient_instructions = 0;
    uint cut_instruction_count = 0;
    uint emit_instruction_count = 0;
    uint barrier_instructions = 0;
    uint interlocked_instructions = 0;
    uint texture_store_instructions = 0;
};

struct Shader {
    std::string name;
    std::string hash;
    std::vector<std::byte> dxil;
    std::vector<std::byte> pdb;
    std::vector<char> disassembly;
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

struct ShaderTask {
    std::string_view path;
    std::string_view name;
    std::vector<std::string_view> entry_points;
};

auto bake_shaders(std::string_view buffet_dir, std::span<const ShaderTask> shader_tasks)
    -> std::vector<Shader>;

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
