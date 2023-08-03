#include "debug_draw.hpp"
#include "shaders.hpp"

namespace fb {

GpuDebugDraw::GpuDebugDraw(Dx& dx, std::string_view name) :
    _root_signature(dx, dx_name(name, NAME)),
    _descriptors(dx, dx_name(name, NAME)) {
    // Shaders.
    GpuShader vertex_shader;
    GpuShader pixel_shader;
    {
        GpuShaderCompiler sc;
        auto source = read_whole_file("shaders/debug_draw.hlsl");
        vertex_shader = sc.compile(NAME, GpuShaderType::Vertex, "vs_main", source);
        pixel_shader = sc.compile(NAME, GpuShaderType::Pixel, "ps_main", source);
    }

    // Pipeline state.
    {
        using CommonStates = DirectX::DX12::CommonStates;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {
            .pRootSignature = _root_signature.get(),
            .VS = vertex_shader.bytecode(),
            .PS = pixel_shader.bytecode(),
            .BlendState = CommonStates::AlphaBlend,
            .SampleMask = UINT_MAX,
            .RasterizerState = CommonStates::CullNone,
            .DepthStencilState = CommonStates::DepthDefault,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
            .NumRenderTargets = 1,
            .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
            .DSVFormat = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc = {.Count = 1, .Quality = 0},
        };
        FAIL_FAST_IF_FAILED(
            dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&_pipeline_state)));
        dx_set_name(_pipeline_state, dx_name(name, NAME, "Pipeline State"));
    }

    // Frame resources.
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        auto& frame = _frames[i];

        frame._constant_buffer.create(dx, 1, dx_name(name, NAME, "Constant Buffer", i));
        frame._constant_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        auto cbv_desc = frame._constant_buffer.cbv_desc();
        dx.device->CreateConstantBufferView(&cbv_desc, frame._constant_buffer_descriptor.cpu());

        frame._lines_buffer.create(dx, MAX_LINE_COUNT, dx_name(name, NAME, "Lines Buffer", i));
        frame._lines_buffer_descriptor = _descriptors.cbv_srv_uav().alloc();
        auto srv_desc = frame._lines_buffer.srv_desc();
        dx.device->CreateShaderResourceView(
            frame._lines_buffer.resource(),
            &srv_desc,
            frame._lines_buffer_descriptor.cpu());
    }
}

auto GpuDebugDraw::begin(uint32_t frame_index) -> void {
    _frame_index = frame_index;
    _lines.clear();
}

auto GpuDebugDraw::transform(const Matrix& transform) -> void {
    _constants.transform = transform;
}

auto GpuDebugDraw::line(const Vector3& a, const Vector3& b, ColorRgba8 color) -> void {
    FAIL_FAST_IF(_lines.size() >= MAX_LINE_COUNT);
    _lines.push_back({a, color});
    _lines.push_back({b, color});
}

auto GpuDebugDraw::axes() -> void {
    line(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), COLOR_RED);
    line(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), COLOR_GREEN);
    line(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
}

auto GpuDebugDraw::end() -> void {
    auto& frame = _frames[_frame_index];
    memcpy(frame._constant_buffer.ptr(), &_constants, sizeof(_constants));
    memcpy(frame._lines_buffer.ptr(), _lines.data(), _lines.size() * sizeof(Vertex));
}

auto GpuDebugDraw::render(Dx& dx) -> void {
    auto* cmd = dx.command_list.get();

    ID3D12DescriptorHeap* heaps[] = {
        _descriptors.cbv_srv_uav().heap(),
        _descriptors.sampler().heap()};
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);
    cmd->SetGraphicsRootSignature(_root_signature.get());
    cmd->SetPipelineState(_pipeline_state.get());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    const auto& frame = _frames[_frame_index];
    GpuBindings bindings;
    bindings.push(frame._constant_buffer_descriptor);
    bindings.push(frame._lines_buffer_descriptor);
    cmd->SetGraphicsRoot32BitConstants(0, bindings.capacity(), bindings.ptr(), 0);
    cmd->DrawInstanced((uint32_t)_lines.size(), 1, 0, 0);
}

}  // namespace fb
