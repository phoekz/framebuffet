#include "tree.hpp"

#pragma warning(push)
#pragma warning(disable : 4100)

namespace fb::demos::tree {

tree::TreeDemo::TreeDemo(
    GpuDevice& device,
    const baked::Assets& assets,
    const baked::Shaders& shaders
)
    : _render_targets(
        device,
        {
            .size = device.swapchain_size(),
            .color_format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .clear_color = CLEAR_COLOR,
            .sample_count = 1,
        },
        NAME
    )
    , _debug_draw(device, shaders, _render_targets, NAME) {
    // Constants.
    _constants.create(device, 1, dx_name(NAME, "Constants"));

    // Geometry.
    const auto tree = assets.coconut_tree_mesh();
    const auto plane = assets.sand_plane_mesh();
    _tree_vertices.create_with_data(device, tree.vertices, dx_name(NAME, "Tree", "Vertices"));
    _tree_indices.create_with_data(device, tree.indices, dx_name(NAME, "Tree", "Indices"));
    _plane_vertices.create_with_data(device, plane.vertices, dx_name(NAME, "Plane", "Vertices"));
    _plane_indices.create_with_data(device, plane.indices, dx_name(NAME, "Plane", "Indices"));

    // Texture.
    {
        const auto& texture = assets.coconut_tree_texture();
        _tree_texture.create(
            device,
            GpuTextureDesc {
                .format = texture.format,
                .width = texture.width,
                .height = texture.height,
            },
            dx_name(NAME, "Tree", "Texture")
        );
        device.transfer().resource(
            _tree_texture.resource(),
            D3D12_SUBRESOURCE_DATA {
                .pData = texture.data.data(),
                .RowPitch = texture.row_pitch,
                .SlicePitch = texture.slice_pitch},
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    }
    {
        const auto& texture = assets.sand_plane_texture();
        _plane_texture.create(
            device,
            GpuTextureDesc {
                .format = texture.format,
                .width = texture.width,
                .height = texture.height,
            },
            dx_name(NAME, "Plane", "Texture")
        );
        device.transfer().resource(
            _plane_texture.resource(),
            D3D12_SUBRESOURCE_DATA {
                .pData = texture.data.data(),
                .RowPitch = texture.row_pitch,
                .SlicePitch = texture.slice_pitch},
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    }

    // Depth.
    constexpr DXGI_FORMAT DEPTH_RAW_FORMAT = DXGI_FORMAT_R32_TYPELESS;
    constexpr DXGI_FORMAT DEPTH_SRV_FORMAT = DXGI_FORMAT_R32_FLOAT;
    constexpr DXGI_FORMAT DEPTH_DSV_FORMAT = DXGI_FORMAT_D32_FLOAT;
    _shadow_depth.create(
        device,
        GpuTextureDesc {
            .format = DEPTH_RAW_FORMAT,
            .width = SHADOW_MAP_SIZE,
            .height = SHADOW_MAP_SIZE,
            .clear_value =
                D3D12_CLEAR_VALUE {
                    .Format = DEPTH_DSV_FORMAT,
                    .DepthStencil = {1.0f, 0},
                },
            .srv_format = DEPTH_SRV_FORMAT,
            .dsv_format = DEPTH_DSV_FORMAT,
        },
        dx_name(NAME, "Shadow", "Depth")
    );

    // Pipelines.
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.tree_shadow_vs())
        .depth_stencil_format(DEPTH_DSV_FORMAT)
        .build(device, _shadow_pipeline, dx_name(NAME, "Shadow", "Pipeline"));
    GpuPipelineBuilder()
        .primitive_topology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .vertex_shader(shaders.tree_draw_vs())
        .pixel_shader(shaders.tree_draw_ps())
        .render_target_formats({_render_targets.color_format()})
        .depth_stencil_format(_render_targets.depth_format())
        .build(device, _draw_pipeline, dx_name(NAME, "Draw", "Pipeline"));
}

static float light_projection_size = 15.0f;
static float light_longitude = 0.0f;
static float light_latitude = rad_from_deg(30.0f);
static float light_distance = 15.0f;
static float shadow_near_plane = 0.1f;
static float shadow_far_plane = 100.0f;
static float camera_distance = 10.0f;
static float camera_fov = rad_from_deg(45.0f);
static float camera_latitude = rad_from_deg(30.0f);
static float camera_longitude = rad_from_deg(0.0f);

auto TreeDemo::gui(const GuiDesc& desc) -> void {
    auto* cb = _constants.ptr();
    ImGui::SliderFloat("Ambient", &cb->ambient_light, 0.0f, 1.0f);
    ImGui::SliderFloat("Light Projection Size", &light_projection_size, 1.0f, 200.0f);
    ImGui::SliderAngle("Light Longitude", &light_longitude, 0.0f, 360.0f);
    ImGui::SliderAngle("Light Latitude", &light_latitude, 0.0f, 180.0f);
    ImGui::SliderFloat("Light Distance", &light_distance, 1.0f, 200.0f);
    ImGui::SliderFloat("Shadow Near Plane", &shadow_near_plane, 0.0f, 10.0f);
    ImGui::SliderFloat("Shadow Far Plane", &shadow_far_plane, 1.0f, 100.0f);
    ImGui::SliderFloat("Camera Distance", &camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &camera_longitude, 0.0f, 360.0f);
}

auto TreeDemo::update(const UpdateDesc& desc) -> void {
    auto* cb = _constants.ptr();

    // Update light angle.
    {
        light_longitude += desc.delta_time;
        if (light_longitude > PI * 2.0f) {
            light_longitude -= PI * 2.0f;
        }
    }

    // Light direction.
    cb->light_direction = dir_from_lonlat(light_longitude, light_latitude);

    // Shadow pass - constants.
    {
        auto projection = Float4x4::CreateOrthographic(
            light_projection_size,
            light_projection_size,
            shadow_near_plane,
            shadow_far_plane
        );
        auto eye = light_distance * cb->light_direction;
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);

        cb->light_transform = view * projection;
    }

    // Main pass - constants.
    Float4x4 transform;
    {
        auto projection =
            Float4x4::CreatePerspectiveFieldOfView(camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
        auto eye = camera_distance * dir_from_lonlat(camera_longitude, camera_latitude);
        auto at = Float3(0.0f, 3.0f, 0.0f);
        auto view = Float4x4::CreateLookAt(eye, at, Float3::Up);
        transform = view * projection;

        cb->transform = transform;
    }

    // Debug.
    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(transform);
        _debug_draw.axes();
        _debug_draw.line(Float3(), 16.0f * cb->light_direction, COLOR_YELLOW);
        _debug_draw.end();
    }
}

auto TreeDemo::render(GpuDevice& device, GpuCommandList& cmd) -> void {
    // Shadow pass.
    {
        cmd.set_graphics();
        _shadow_depth.transition(cmd, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        cmd.set_viewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_scissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
        cmd.set_rtv_dsv(std::nullopt, _shadow_depth.dsv_descriptor());
        cmd.clear_dsv(_shadow_depth.dsv_descriptor(), 1.0f);
        cmd.set_graphics_constants(Bindings {
            _constants.cbv_descriptor().index(),
            _tree_vertices.srv_descriptor().index(),
        });
        cmd.set_pipeline(_shadow_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd.set_index_buffer(_tree_indices.index_buffer_view());
        cmd.draw_indexed_instanced(_tree_indices.element_size(), 1, 0, 0, 0);
        _shadow_depth.transition(cmd, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // Main pass.
    {
        cmd.set_graphics();
        _render_targets.begin(device, cmd);
        _debug_draw.render(device, cmd);
        cmd.set_pipeline(_draw_pipeline);
        cmd.set_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        cmd.set_graphics_constants(Bindings {
            _constants.cbv_descriptor().index(),
            _tree_vertices.srv_descriptor().index(),
            _tree_texture.srv_descriptor().index(),
            _shadow_depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(_tree_indices.index_buffer_view());
        cmd.draw_indexed_instanced(_tree_indices.element_size(), 1, 0, 0, 0);

        cmd.set_graphics_constants(Bindings {
            _constants.cbv_descriptor().index(),
            _plane_vertices.srv_descriptor().index(),
            _plane_texture.srv_descriptor().index(),
            _shadow_depth.srv_descriptor().index(),
        });
        cmd.set_index_buffer(_plane_indices.index_buffer_view());
        cmd.draw_indexed_instanced(_plane_indices.element_size(), 1, 0, 0, 0);

        _render_targets.end(device, cmd);
    }
}

} // namespace fb::demos::tree
