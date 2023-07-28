#include "shaders.hpp"

namespace fb {

D3D12_SHADER_BYTECODE Shader::bytecode() {
    return {
        .pShaderBytecode = blob->GetBufferPointer(),
        .BytecodeLength = blob->GetBufferSize(),
    };
}

void Shader::release() {
    blob = nullptr;
}

ShaderCompiler::ShaderCompiler() {
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
    utils->CreateDefaultIncludeHandler(&include_handler);
}

Shader ShaderCompiler::compile(
    std::string_view name,
    ShaderType type,
    std::string_view entry_point,
    std::span<std::byte> source) {
    // Note: remember to set PIX PDB search path correctly for shader debugging to work.

    // Shader profile.
    LPCWSTR shader_profile = nullptr;
    // clang-format off
    switch (type) {
        case ShaderType::Compute: shader_profile = L"cs_6_7"; break;
        case ShaderType::Vertex: shader_profile = L"vs_6_7"; break;
        case ShaderType::Pixel: shader_profile = L"ps_6_7"; break;
    }
    // clang-format on

    // Shader arguments.
    std::wstring shader_name = fb::to_wstr(name);
    std::wstring shader_bin = std::format(L"{}.bin", shader_name);
    std::wstring shader_entry = fb::to_wstr(entry_point);
    LPCWSTR shader_args[] = {
        // clang-format off
        shader_name.c_str(),
        L"-E", shader_entry.c_str(),
        L"-T", shader_profile,
        L"-HV", L"2021",
        L"-Zi",
        L"-Fo", shader_bin.c_str(),
        L"-Fd", L".\\shaders\\",
        L"-Qstrip_reflect",
        // clang-format on
    };

    // Compile.
    DxcBuffer source_buffer = {
        .Ptr = source.data(),
        .Size = source.size(),
        .Encoding = DXC_CP_ACP,
    };
    fb::ComPtr<IDxcResult> result;
    fb::ComPtr<IDxcBlobUtf8> errors;
    IDxcBlob* blob = nullptr;
    fb::ComPtr<IDxcBlob> pdb;
    fb::ComPtr<IDxcBlobUtf16> pdb_name;
    FAIL_FAST_IF_FAILED(compiler->Compile(
        &source_buffer,
        shader_args,
        _countof(shader_args),
        include_handler.get(),
        IID_PPV_ARGS(&result)));
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));
    if (errors && errors->GetStringLength() != 0) {
        FAIL_FAST_MSG("Failed to compile %ws\n%s", shader_name.c_str(), errors->GetStringPointer());
    }
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr));
    FAIL_FAST_IF_FAILED(result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdb_name));

    // Write PDB.
    {
        std::wstring path;
        path.append(L"shaders\\");
        path.append(pdb_name->GetStringPointer());

        FILE* file = nullptr;
        _wfopen_s(&file, path.c_str(), L"wb");
        FAIL_FAST_IF_NULL_MSG(file, "Failed to create file %ws", path.c_str());

        fwrite(pdb->GetBufferPointer(), pdb->GetBufferSize(), 1, file);

        fclose(file);
    }

    // Result.
    return Shader {blob};
}

}  // namespace fb
