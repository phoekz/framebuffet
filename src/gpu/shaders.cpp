#include "shaders.hpp"
#include <d3d12shader.h>

namespace fb {

static auto shader_type_name(GpuShaderType type) -> std::string_view {
    switch (type) {
        using enum GpuShaderType;
        case Compute: return "Compute"sv;
        case Vertex: return "Vertex"sv;
        case Pixel: return "Pixel"sv;
        default: return "Unknown"sv;
    }
}

GpuShaderCompiler::GpuShaderCompiler() {
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_utils));
    _utils->CreateDefaultIncludeHandler(&_include_handler);
}

static auto
analyze(std::string_view name, GpuShaderType shader_type, ComPtr<ID3D12ShaderReflection> reflection)
    -> void {
    // Get reflection data.
    D3D12_SHADER_DESC shader_desc;
    reflection->GetDesc(&shader_desc);
    FB_LOG_INFO(
        "Shader: {}, Type: {}, InstructionCount: {}",
        name,
        shader_type_name(shader_type),
        shader_desc.InstructionCount);

    for (UINT cb_index = 0; cb_index < shader_desc.ConstantBuffers; ++cb_index) {
        auto cb = reflection->GetConstantBufferByIndex(cb_index);
        D3D12_SHADER_BUFFER_DESC shader_buffer_desc;
        FB_ASSERT_HR(cb->GetDesc(&shader_buffer_desc));
        FB_LOG_INFO(
            "  ConstantBuffer: {}, Variables: {}, Size: {}",
            shader_buffer_desc.Name,
            shader_buffer_desc.Variables,
            shader_buffer_desc.Size);
        for (UINT var_index = 0; var_index < shader_buffer_desc.Variables; ++var_index) {
            auto var = cb->GetVariableByIndex(var_index);
            D3D12_SHADER_VARIABLE_DESC shader_variable_desc;
            FB_ASSERT_HR(var->GetDesc(&shader_variable_desc));
            FB_LOG_INFO(
                "    Name: {}, Size: {}",
                shader_variable_desc.Name,
                shader_variable_desc.Size);

            auto type = var->GetType();
            D3D12_SHADER_TYPE_DESC shader_type_desc;
            FB_ASSERT_HR(type->GetDesc(&shader_type_desc));
            FB_LOG_INFO(
                "      Name: {}, Members: {}",
                shader_type_desc.Name,
                shader_type_desc.Members);
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
                    shader_member_type_desc.Offset);
            }
        }
    }
    FB_LOG_INFO("");
}

auto GpuShaderCompiler::compile(
    std::string_view name,
    GpuShaderType type,
    std::string_view entry_point,
    std::span<const std::byte> source,
    bool debug) -> GpuShaderBytecode {
    // Note: remember to set PIX PDB search path correctly for shader debugging to work.

    // Shader profile.
    LPCWSTR shader_profile = nullptr;
    // clang-format off
    switch (type) {
        case GpuShaderType::Compute: shader_profile = L"cs_6_7"; break;
        case GpuShaderType::Vertex: shader_profile = L"vs_6_7"; break;
        case GpuShaderType::Pixel: shader_profile = L"ps_6_7"; break;
    }
    // clang-format on

    // Shader arguments.
    std::wstring shader_name = fb::to_wstr(name);
    std::wstring shader_bin = std::format(L"{}.bin", shader_name);
    std::wstring shader_entry = fb::to_wstr(entry_point);
    auto shader_args = std::to_array<LPCWSTR>({
        // clang-format off
        shader_name.c_str(),
        L"-E", shader_entry.c_str(),
        L"-T", shader_profile,
        L"-HV", L"2021",
        L"-Zi",
        L"-Fo", shader_bin.c_str(),
        L"-Fd", L".\\shaders\\",
        L"-WX",
        L"-all_resources_bound",
        // clang-format on
    });

    // Compile.
    DxcBuffer source_buffer = {
        .Ptr = source.data(),
        .Size = source.size(),
        .Encoding = DXC_CP_ACP,
    };
    ComPtr<IDxcResult> result;
    ComPtr<IDxcBlobUtf8> errors;
    IDxcBlob* blob = nullptr;
    ComPtr<IDxcBlob> pdb;
    ComPtr<IDxcBlobUtf16> pdb_name;
    FB_ASSERT_HR(_compiler->Compile(
        &source_buffer,
        shader_args.data(),
        (uint32_t)shader_args.size(),
        _include_handler.get(),
        IID_PPV_ARGS(&result)));
    FB_ASSERT_HR(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));
    if (errors && errors->GetStringLength() != 0) {
        FB_LOG_ERROR("Failed to compile {}", errors->GetStringPointer());
        FB_FATAL();
    }
    FB_ASSERT_HR(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr));
    FB_ASSERT_HR(result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdb_name));

    // Write DXIL.
    {
        ComPtr<IDxcBlob> shader_hash_blob;
        FB_ASSERT_HR(
            result->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&shader_hash_blob), nullptr));
        DxcShaderHash* shader_hash = (DxcShaderHash*)shader_hash_blob->GetBufferPointer();
        std::ostringstream oss;
        for (int i = 0; i < 16; ++i) {
            oss << std::format("{:02x}", shader_hash->HashDigest[i]);
        }

        std::wstring wpath;
        wpath.append(L"shaders\\");
        wpath.append(to_wstr(oss.str()));
        wpath.append(L".dxil");

        FILE* file = nullptr;
        _wfopen_s(&file, wpath.c_str(), L"wb");
        FB_ASSERT_MSG(file != nullptr, "Failed to create file {}", from_wstr(wpath));
        fwrite(blob->GetBufferPointer(), blob->GetBufferSize(), 1, file);
        fclose(file);
    }

    // Write PDB.
    {
        std::wstring wpath;
        wpath.append(L"shaders\\");
        wpath.append(pdb_name->GetStringPointer());

        FILE* file = nullptr;
        _wfopen_s(&file, wpath.c_str(), L"wb");
        FB_ASSERT_MSG(file != nullptr, "Failed to create file {}", from_wstr(wpath));
        fwrite(pdb->GetBufferPointer(), pdb->GetBufferSize(), 1, file);
        fclose(file);
    }

    // Reflection.
    if (debug) {
        ComPtr<IDxcBlob> reflection_blob;
        FB_ASSERT_HR(
            result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflection_blob), nullptr));
        DxcBuffer reflection_buffer;
        reflection_buffer.Encoding = DXC_CP_ACP;
        reflection_buffer.Ptr = reflection_blob->GetBufferPointer();
        reflection_buffer.Size = reflection_blob->GetBufferSize();
        ComPtr<ID3D12ShaderReflection> reflection;
        FB_ASSERT_HR(_utils->CreateReflection(&reflection_buffer, IID_PPV_ARGS(&reflection)));
        analyze(name, type, reflection);
    }

    // Output.
    GpuShaderBytecode result_blob;
    result_blob.resize(blob->GetBufferSize());
    memcpy(result_blob.data(), blob->GetBufferPointer(), blob->GetBufferSize());
    return result_blob;
}

}  // namespace fb
