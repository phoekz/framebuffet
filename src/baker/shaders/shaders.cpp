#include "shaders.hpp"
#include "../utils/names.hpp"

#include <d3d12shader.h>
#include <sstream>

namespace fb {

auto ShaderCounters::to_comment_string() const -> std::string {
    std::ostringstream oss;
    uint lines = 0;
#define add_if_not_zero(name)                  \
    if (name != 0) {                           \
        if (lines > 0) {                       \
            oss << "\n";                       \
        }                                      \
        oss << "// " << #name << ": " << name; \
        lines++;                               \
    }
    add_if_not_zero(constant_buffers);
    add_if_not_zero(bound_resources);
    add_if_not_zero(input_parameters);
    add_if_not_zero(output_parameters);
    add_if_not_zero(instruction_count);
    add_if_not_zero(temp_array_count);
    add_if_not_zero(dynamic_flow_control_count);
    add_if_not_zero(array_instruction_count);
    add_if_not_zero(float_instruction_count);
    add_if_not_zero(int_instruction_count);
    add_if_not_zero(uint_instruction_count);
    add_if_not_zero(texture_normal_instructions);
    add_if_not_zero(texture_load_instructions);
    add_if_not_zero(texture_comp_instructions);
    add_if_not_zero(texture_bias_instructions);
    add_if_not_zero(texture_gradient_instructions);
    add_if_not_zero(cut_instruction_count);
    add_if_not_zero(emit_instruction_count);
    add_if_not_zero(barrier_instructions);
    add_if_not_zero(interlocked_instructions);
    add_if_not_zero(texture_store_instructions);
#undef add_if_not_zero
    return oss.str();
}

static auto shader_type_name(ShaderType type) -> std::string_view {
    switch (type) {
        using enum ShaderType;
        case Compute: return "Compute"sv;
        case Vertex: return "Vertex"sv;
        case Pixel: return "Pixel"sv;
        default: return "Unknown"sv;
    }
}

ShaderCompiler::ShaderCompiler() {
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_utils));
    _utils->CreateDefaultIncludeHandler(&_include_handler);
}

static auto
analyze(std::string_view name, ShaderType shader_type, ComPtr<ID3D12ShaderReflection> reflection)
    -> void {
    // Get reflection data.
    D3D12_SHADER_DESC shader_desc;
    reflection->GetDesc(&shader_desc);
    FB_LOG_INFO(
        "Shader: {}, Type: {}, InstructionCount: {}",
        name,
        shader_type_name(shader_type),
        shader_desc.InstructionCount
    );

    for (UINT cb_index = 0; cb_index < shader_desc.ConstantBuffers; ++cb_index) {
        auto cb = reflection->GetConstantBufferByIndex(cb_index);
        D3D12_SHADER_BUFFER_DESC shader_buffer_desc;
        FB_ASSERT_HR(cb->GetDesc(&shader_buffer_desc));
        FB_LOG_INFO(
            "  ConstantBuffer: {}, Variables: {}, Size: {}",
            shader_buffer_desc.Name,
            shader_buffer_desc.Variables,
            shader_buffer_desc.Size
        );
        for (UINT var_index = 0; var_index < shader_buffer_desc.Variables; ++var_index) {
            auto var = cb->GetVariableByIndex(var_index);
            D3D12_SHADER_VARIABLE_DESC shader_variable_desc;
            FB_ASSERT_HR(var->GetDesc(&shader_variable_desc));
            FB_LOG_INFO(
                "    Name: {}, Size: {}",
                shader_variable_desc.Name,
                shader_variable_desc.Size
            );

            auto type = var->GetType();
            D3D12_SHADER_TYPE_DESC shader_type_desc;
            FB_ASSERT_HR(type->GetDesc(&shader_type_desc));
            FB_LOG_INFO(
                "      Name: {}, Members: {}",
                shader_type_desc.Name,
                shader_type_desc.Members
            );
            for (UINT member_index = 0; member_index < shader_type_desc.Members; ++member_index) {
                auto member = type->GetMemberTypeByIndex(member_index);
                auto member_name = type->GetMemberTypeName(member_index);
                D3D12_SHADER_TYPE_DESC shader_member_type_desc;
                FB_ASSERT_HR(member->GetDesc(&shader_member_type_desc));
                FB_LOG_INFO(
                    "        {} {:16} // idx={} offset={}",
                    shader_member_type_desc.Name,
                    member_name,
                    member_index,
                    shader_member_type_desc.Offset
                );
            }
        }
    }
    FB_LOG_INFO("");
}

auto ShaderCompiler::compile(
    std::string_view name,
    ShaderType type,
    std::string_view entry_point,
    Span<const std::byte> source,
    bool debug
) const -> Shader {
    // Note: remember to set PIX PDB search path correctly for shader debugging to work.

    // Shader profile.
    LPCWSTR shader_profile = nullptr;
    // clang-format off
    switch (type) {
        case ShaderType::Compute: shader_profile = L"cs_6_8"; break;
        case ShaderType::Vertex: shader_profile = L"vs_6_8"; break;
        case ShaderType::Pixel: shader_profile = L"ps_6_8"; break;
        default: FB_FATAL();
    }
    // clang-format on

    // Shader arguments.
    std::wstring shader_name = fb::to_wstr(name);
    std::wstring shader_bin = std::format(L"{}.bin", shader_name);
    std::wstring shader_entry = fb::to_wstr(entry_point);
    auto shader_args = std::to_array<LPCWSTR>({
        // clang-format off
        shader_name.c_str(),
        L"-E", shader_entry.c_str(),      // Entry point name.
        L"-T", shader_profile,            // Target profile.
        L"-HV", L"202x",                  // HLSL version.
        L"-Zi",                           // Debug information.
        L"-Fo", shader_bin.c_str(),       // Output object file.
        L"-Fd", L".\\shaders\\",          // Write debug information to the given file.
        L"-Wall",                         // Enable all warnings.
        L"-Wextra",                       // Enable extra warnings.
        L"-WX",                           // Treat warnings as errors.
        L"-Wconversion",
        L"-Wdouble-promotion",
        L"-all-resources-bound",          // Enable aggressive flattening.
        L"-enable-16bit-types",           // Enable 16-bit types and disable min precision types.
        L"-I", FB_BAKER_SOURCE_DIR_WIDE "/src",
        // clang-format on
    });

    // Compile.
    DxcBuffer source_buffer = {
        .Ptr = source.data(),
        .Size = source.size(),
        .Encoding = DXC_CP_ACP,
    };
    ComPtr<IDxcResult> compile_result;
    ComPtr<IDxcBlobUtf8> compile_errors;
    FB_ASSERT_HR(_compiler->Compile(
        &source_buffer,
        shader_args.data(),
        (uint)shader_args.size(),
        _include_handler.get(),
        IID_PPV_ARGS(&compile_result)
    ));
    FB_ASSERT_HR(compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&compile_errors), nullptr));
    if (compile_errors && compile_errors->GetStringLength() != 0) {
        FB_LOG_ERROR("Failed to compile {}:\n{}", name, compile_errors->GetStringPointer());
        FB_FATAL();
    }

    // Get DXIL.
    std::vector<std::byte> dxil;
    std::string hash;
    {
        IDxcBlob* dxil_blob = nullptr;
        FB_ASSERT_HR(compile_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&dxil_blob), nullptr));
        dxil.resize(dxil_blob->GetBufferSize());
        memcpy(dxil.data(), dxil_blob->GetBufferPointer(), dxil_blob->GetBufferSize());

        ComPtr<IDxcBlob> shader_hash_blob;
        FB_ASSERT_HR(
            compile_result->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&shader_hash_blob), nullptr)
        );
        auto* shader_hash = (DxcShaderHash*)shader_hash_blob->GetBufferPointer();
        std::ostringstream oss;
        for (auto byte : shader_hash->HashDigest) {
            oss << std::format("{:02x}", byte);
        }
        hash = oss.str();
    }

    // Get PDB.
    std::vector<std::byte> pdb;
    {
        ComPtr<IDxcBlob> pdb_blob;
        ComPtr<IDxcBlobUtf16> pdb_name;
        FB_ASSERT_HR(compile_result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb_blob), &pdb_name));

        pdb.resize(pdb_blob->GetBufferSize());
        memcpy(pdb.data(), pdb_blob->GetBufferPointer(), pdb_blob->GetBufferSize());
    }

    // Disassemble.
    std::vector<char> disassembly;
    {
        DxcBuffer dxil_buffer = {
            .Ptr = dxil.data(),
            .Size = dxil.size(),
            .Encoding = DXC_CP_ACP,
        };
        ComPtr<IDxcResult> disassemble_result;
        FB_ASSERT_HR(_compiler->Disassemble(&dxil_buffer, IID_PPV_ARGS(&disassemble_result)));
        ComPtr<IDxcBlobUtf8> disassemble_blob;
        FB_ASSERT_HR(disassemble_result->GetOutput(
            DXC_OUT_DISASSEMBLY,
            IID_PPV_ARGS(&disassemble_blob),
            nullptr
        ));
        const size_t string_length = disassemble_blob->GetStringLength();
        const char* string = disassemble_blob->GetStringPointer();
        disassembly.resize(string_length + 1, '\0');
        memcpy(disassembly.data(), string, string_length);
    }

    // Reflection.
    ComPtr<ID3D12ShaderReflection> reflection;
    {
        ComPtr<IDxcBlob> blob;
        FB_ASSERT_HR(compile_result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&blob), nullptr));
        DxcBuffer buffer;
        buffer.Encoding = DXC_CP_ACP;
        buffer.Ptr = blob->GetBufferPointer();
        buffer.Size = blob->GetBufferSize();
        FB_ASSERT_HR(_utils->CreateReflection(&buffer, IID_PPV_ARGS(&reflection)));
    }

    // Shader desc.
    ShaderCounters counters;
    {
        D3D12_SHADER_DESC desc;
        FB_ASSERT_HR(reflection->GetDesc(&desc));
        counters.constant_buffers = desc.ConstantBuffers;
        counters.bound_resources = desc.BoundResources;
        counters.input_parameters = desc.InputParameters;
        counters.output_parameters = desc.OutputParameters;
        counters.instruction_count = desc.InstructionCount;
        counters.temp_array_count = desc.TempArrayCount;
        counters.dynamic_flow_control_count = desc.DynamicFlowControlCount;
        counters.array_instruction_count = desc.ArrayInstructionCount;
        counters.float_instruction_count = desc.FloatInstructionCount;
        counters.int_instruction_count = desc.IntInstructionCount;
        counters.uint_instruction_count = desc.UintInstructionCount;
        counters.texture_normal_instructions = desc.TextureNormalInstructions;
        counters.texture_load_instructions = desc.TextureLoadInstructions;
        counters.texture_comp_instructions = desc.TextureCompInstructions;
        counters.texture_bias_instructions = desc.TextureBiasInstructions;
        counters.texture_gradient_instructions = desc.TextureGradientInstructions;
        counters.cut_instruction_count = desc.CutInstructionCount;
        counters.emit_instruction_count = desc.EmitInstructionCount;
        counters.barrier_instructions = desc.cBarrierInstructions;
        counters.interlocked_instructions = desc.cInterlockedInstructions;
        counters.texture_store_instructions = desc.cTextureStoreInstructions;
    }

    // Debug.
    if (debug) {
        analyze(name, type, reflection);
    }

    // Return shader.
    return Shader {
        .name = std::string(name),
        .hash = hash,
        .dxil = std::move(dxil),
        .pdb = std::move(pdb),
        .disassembly = std::move(disassembly),
        .counters = counters,
    };
}

static auto shader_type_from_entry_point(std::string_view entry_point) -> ShaderType {
    if (entry_point == "vs" || entry_point.ends_with("_vs")) {
        return ShaderType::Vertex;
    } else if (entry_point == "ps" || entry_point.ends_with("_ps")) {
        return ShaderType::Pixel;
    } else if (entry_point == "cs" || entry_point.ends_with("_cs")) {
        return ShaderType::Compute;
    } else {
        FB_FATAL();
    }
}

static auto make_unique_shader_name(std::string_view name, std::string_view entry_point) {
    return std::format("{}_{}", name, entry_point);
}

auto bake_shaders(std::string_view buffet_dir, Span<const ShaderTask> shader_tasks)
    -> std::vector<Shader> {
    const auto compiler = ShaderCompiler();
    auto compiled_shaders = std::vector<Shader>();
    auto unique_names = std::unordered_set<std::string>();
    auto names = UniqueNames();

    size_t shader_entry_point_count = 0;
    for (const auto& shader_task : shader_tasks) {
        shader_entry_point_count += shader_task.entry_points.size();
    }
    size_t shader_entry_point_index = 0;

    FB_LOG_INFO(
        "Baking {} shader tasks ({} entry points)",
        shader_tasks.size(),
        shader_entry_point_count
    );
    for (const auto& shader_task : shader_tasks) {
        const auto hlsl_path = std::format("{}/{}", buffet_dir, shader_task.path);
        const auto hlsl_file = FileBuffer::from_path(hlsl_path);

        for (const auto& entry_point : shader_task.entry_points) {
            // Ensure unique shader names.
            const auto name = names.unique(make_unique_shader_name(shader_task.name, entry_point));

            // Determine shader type.
            const auto type = shader_type_from_entry_point(entry_point);
            FB_ASSERT(type != ShaderType::Unknown);

            // Compile shader.
            auto shader = compiler.compile(name, type, entry_point, hlsl_file.as_span(), false);

            // Log.
            FB_LOG_INFO(
                "{}/{} - {} - {} instructions",
                ++shader_entry_point_index,
                shader_entry_point_count,
                name,
                shader.counters.instruction_count
            );

            // Save shader.
            compiled_shaders.push_back(std::move(shader));
        }
    }

    return compiled_shaders;
}

} // namespace fb
