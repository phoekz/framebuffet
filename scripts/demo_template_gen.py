import os

base_dir = "src/buffet/demos"
demos = [
    ("conras", "Conras"),
]
files = {
    #
    # <demo>.cpp
    #
    "cpp": """#include "{lower_name}.hpp"

namespace fb::demos::{lower_name} {{

auto create(Demo& demo, const CreateDesc& desc) -> void {{
    FB_PERF_FUNC();
    DebugScope debug(NAME);

    // Unpack.
    const auto& kitchen_shaders = desc.baked.kitchen.shaders;
    const auto& shaders = desc.baked.buffet.shaders;
    const auto& assets = desc.baked.buffet.assets;
    auto& device = desc.device;

    // Render targets.
    demo.render_target.create(
        device,
        {{
            .size = device.swapchain().size(),
            .sample_count = SAMPLE_COUNT,
            .colors = COLOR_ATTACHMENTS,
            .depth_stencil = DEPTH_STENCIL_ATTACHMENT,
        }}
    );
    demo.render_target_view.create(demo.render_target.view_desc());

    // Debug draw.
    demo.debug_draw.create(device, kitchen_shaders, demo.render_target_view);

    // Constants.
    demo.constants.create(device, 1, debug.with_name("Constants"));

    (void)shaders;
    (void)assets;
}}

auto gui(Demo& demo, const GuiDesc&) -> void {{
    FB_PERF_FUNC();
    auto& params = demo.parameters;
    ImGui::SliderFloat("Camera Distance", &params.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &params.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &params.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &params.camera_longitude, 0.0f, 360.0f);
    ImGui::SliderFloat("Camera Rotation Speed", &params.camera_rotation_speed, 0.0f, 2.0f);
}}

auto update(Demo& demo, const UpdateDesc& desc) -> void {{
    FB_PERF_FUNC();
    auto& params = demo.parameters;

    // Update camera.
    params.camera_longitude += params.camera_rotation_speed * desc.delta_time;
    if (params.camera_longitude > PI * 2.0f) {{
        params.camera_longitude -= PI * 2.0f;
    }}
    const auto projection =
        float4x4_perspective(params.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
    const auto eye =
        params.camera_distance * float3_from_lonlat(params.camera_longitude, params.camera_latitude);
    const auto view = float4x4_lookat(eye, FLOAT3_ZERO, FLOAT3_UP);
    const auto camera_transform = view * projection;

    // Update debug draw.
    demo.debug_draw.begin(desc.frame_index);
    demo.debug_draw.transform(camera_transform);
    demo.debug_draw.axes();
    demo.debug_draw.end();

    // Update constants.
    demo.constants.buffer(desc.frame_index).ref() = Constants {{
        .transform = camera_transform,
    }};
}}

auto render(Demo& demo, const RenderDesc& desc) -> void {{
    FB_PERF_FUNC();
    auto& [cmd, device, frame_index] = desc;
    cmd.pix_begin("%s - Render", NAME.data());

    cmd.compute_scope([&demo, frame_index](GpuComputeCommandList& cmd) {{
        cmd.pix_begin("Compute");
        cmd.pix_end();
    }});

    cmd.graphics_scope([&demo, frame_index](GpuGraphicsCommandList& cmd) {{
        demo.render_target_view.set_graphics(cmd);

        demo.debug_draw.render(cmd);

        cmd.pix_begin("Draw");
        cmd.pix_end();
    }});

    cmd.pix_end();
}}

}} // namespace fb::demos::{lower_name}
""",
    #
    # <demo>.hpp
    #
    "hpp": """#pragma once

#include "../common.hpp"
#include "{lower_name}.hlsli"

namespace fb::demos::{lower_name} {{

inline constexpr std::string_view NAME = "{upper_name}"sv;
inline constexpr KcnColorAttachmentDescs COLOR_ATTACHMENTS = {{KcnColorAttachmentDesc {{
    .format = DXGI_FORMAT_R8G8B8A8_UNORM,
    .clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}},
}}}};
inline constexpr KcnDepthStencilAttachmentDesc DEPTH_STENCIL_ATTACHMENT = {{
    .format = DXGI_FORMAT_D32_FLOAT,
    .clear_depth = 1.0f,
    .clear_stencil = 0,
}};
inline constexpr uint SAMPLE_COUNT = 1;

struct Parameters {{
    float camera_distance = 3.0f;
    float camera_fov = rad_from_deg(45.0f);
    float camera_latitude = rad_from_deg(30.0f);
    float camera_longitude = rad_from_deg(0.0f);
    float camera_rotation_speed = 0.2f;
}};

struct Demo {{
    Parameters parameters;
    KcnRenderTarget render_target;
    KcnRenderTargetView render_target_view;
    KcnDebugDraw debug_draw;
    KcnMultibuffer<GpuBufferHostCbv<Constants>, FRAME_COUNT> constants;
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
    #
    # <demo>.hlsl
    #
    "hlsl": """#include <buffet/demos/{lower_name}/{lower_name}.hlsli>
#include <kitchen/gpu/samplers.hlsli>
#include <kitchen/kcn/core.hlsli>

ConstantBuffer<Bindings> g_bindings: register(b0);

FB_ATTRIBUTE(numthreads, DISPATCH_X, DISPATCH_Y, DISPATCH_Z)
void todo_cs(fb::ComputeInput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
}}

struct TodoVertexOutput {{
    float4 position: SV_Position;
    float2 texcoord: ATTRIBUTE0;
}};

TodoVertexOutput todo_vs(fb::VertexInput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    TodoVertexOutput output;
    output.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.texcoord = float2(0.0f, 0.0f);
    return output;
}}

fb::PixelOutput<1> todo_ps(TodoVertexOutput input) {{
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];

    fb::PixelOutput<1> output;
    output.color = float4(input.texcoord, 0.0f, 1.0f);
    return output;
}}

""",
    #
    # <demo>.hlsli
    #
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
    float4x4 transform;
    float pad[48];
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
    write_to_file(
        demo_dir + "/" + lower_name + ".cpp",
        files["cpp"].format(lower_name=lower_name, upper_name=upper_name),
    )
    write_to_file(
        demo_dir + "/" + lower_name + ".hpp",
        files["hpp"].format(lower_name=lower_name, upper_name=upper_name),
    )
    write_to_file(
        demo_dir + "/" + lower_name + ".hlsl",
        files["hlsl"].format(lower_name=lower_name, upper_name=upper_name),
    )
    write_to_file(
        demo_dir + "/" + lower_name + ".hlsli",
        files["hlsli"].format(lower_name=lower_name, upper_name=upper_name),
    )

    print(demo_dir + "/" + lower_name + ".cpp")
    print(demo_dir + "/" + lower_name + ".hpp")
    print(demo_dir + "/" + lower_name + ".hlsl")
    print(demo_dir + "/" + lower_name + ".hlsli")
