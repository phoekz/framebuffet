import os

base_dir = "src/buffet/demos"
demos = [
    ("saber", "Saber"),
]
files = {
    "cpp": """#include "{lower_name}.hpp"

namespace fb::demos::{lower_name} {{

auto create(Demo& demo, const CreateDesc& desc) -> void {{
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Create", NAME.data());
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_targets.create(
        device,
        {{
            .size = device.swapchain().size(),
            .color_format = COLOR_FORMAT,
            .color_clear_value = COLOR_CLEAR_VALUE,
            .depth_format = DEPTH_FORMAT,
            .depth_clear_value = DEPTH_CLEAR_VALUE,
            .sample_count = SAMPLE_COUNT,
        }}
    );

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_targets);

    (void)shaders;
    (void)assets;
}}

auto gui(Demo& demo, const GuiDesc&) -> void {{
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Gui", NAME.data());
    auto& params = demo.parameters;
    (void)params;
}}

auto update(Demo& demo, const UpdateDesc& desc) -> void {{
    PIXScopedEvent(PIX_COLOR_DEFAULT, "%s - Update", NAME.data());
    auto& params = demo.parameters;
    (void)params;
    (void)desc;
}}

auto render(Demo& demo, const RenderDesc& desc) -> void {{
    auto& [cmd, device, frame_index] = desc;
    cmd.begin_pix("%s - Render", NAME.data());

    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {{
        cmd.begin_pix("Compute");
        cmd.end_pix();
    }});

    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {{
        demo.render_targets.set(cmd);

        demo.debug_draw.render(cmd);

        cmd.begin_pix("Draw");
        cmd.end_pix();
    }});

    cmd.end_pix();
}}

}} // namespace fb::demos::{lower_name}
""",

"hpp": """#pragma once

#include "../common.hpp"
#include "{lower_name}.hlsli"

namespace fb::demos::{lower_name} {{

inline constexpr std::string_view NAME = "{upper_name}"sv;
inline constexpr DXGI_FORMAT COLOR_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
inline constexpr float4 COLOR_CLEAR_VALUE = {{0.0f, 0.0f, 0.0f, 1.0f}};
inline constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
inline constexpr float DEPTH_CLEAR_VALUE = 1.0f;
inline constexpr uint SAMPLE_COUNT = 1;

struct Parameters {{

}};

struct Demo {{
    Parameters parameters;
    RenderTargets render_targets;
    DebugDraw debug_draw;
}};

struct CreateDesc {{
    const Baked& baked;
    GpuDevice& device;
}};

auto create(Demo& demo, const CreateDesc& desc) -> void;
auto gui(Demo& demo, const GuiDesc& desc) -> void;
auto update(Demo& demo, const UpdateDesc& desc) -> void;
auto render(Demo& demo, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demo& demo, A& arc) -> void {{
    arc& demo.parameters;
}}

}} // namespace fb::demos::{lower_name}
""",

"hlsl": """#include <buffet/demos/{lower_name}/{lower_name}.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/graphics/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void todo_cs(FbComputeInput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
}}

struct VertexOutput {{
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
}};

VertexOutput todo_vs(FbVertexInput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    VertexOutput output;
    output.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.texcoord = float2(0.0f, 0.0f);
    return output;
}}

FbPixelOutput<1> todo_ps(VertexOutput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    FbPixelOutput<1> output;
    output.color = float4(input.texcoord, 0.0f, 1.0f);
    return output;
}}

""",

"hlsli": """#pragma once

#include <kitchen/gpu/hlsl_cpp.hlsli>

FB_NAMESPACE_BEGIN(fb::demos::{lower_name})

FB_CONSTANT uint DISPATCH_X = 1;
FB_CONSTANT uint DISPATCH_Y = 1;
FB_CONSTANT uint DISPATCH_Z = 1;

struct Bindings {{
    uint constants;
}};

struct Constants {{
    float pad[64];
}};

FB_NAMESPACE_END(fb::demos::{lower_name})

""",
}

def write_to_file(path, content):
    with open(path, "w") as f:
        f.write(content)

for lower_name, upper_name in demos:
    # Paths.
    demo_dir = base_dir + "/" + lower_name
    if not os.path.exists(demo_dir):
        os.makedirs(demo_dir)

    # Files.
    write_to_file(demo_dir + "/" + lower_name + ".cpp", files["cpp"].format(lower_name=lower_name, upper_name=upper_name))
    write_to_file(demo_dir + "/" + lower_name + ".hpp", files["hpp"].format(lower_name=lower_name, upper_name=upper_name))
    write_to_file(demo_dir + "/" + lower_name + ".hlsl", files["hlsl"].format(lower_name=lower_name, upper_name=upper_name))
    write_to_file(demo_dir + "/" + lower_name + ".hlsli", files["hlsli"].format(lower_name=lower_name, upper_name=upper_name))

    print(demo_dir + "/" + lower_name + ".cpp")
    print(demo_dir + "/" + lower_name + ".hpp")
    print(demo_dir + "/" + lower_name + ".hlsl")
    print(demo_dir + "/" + lower_name + ".hlsli")
