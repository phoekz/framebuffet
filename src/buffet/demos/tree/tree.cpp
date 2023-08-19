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
            .sample_count = 4,
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
        .sample_desc(_render_targets.sample_desc())
        .build(device, _draw_pipeline, dx_name(NAME, "Draw", "Pipeline"));
}

auto TreeDemo::gui(const GuiDesc& desc) -> void {
    auto& p = _parameters;
    ImGui::SliderFloat("Ambient Light", &p.ambient_light, 0.0f, 1.0f);
    ImGui::SliderFloat("Light Projection Size", &p.light_projection_size, 1.0f, 200.0f);
    ImGui::SliderAngle("Light Longitude", &p.light_longitude, 0.0f, 360.0f);
    ImGui::SliderAngle("Light Latitude", &p.light_latitude, 0.0f, 180.0f);
    ImGui::SliderFloat("Light Distance", &p.light_distance, 1.0f, 200.0f);
    ImGui::SliderFloat("Shadow Near Plane", &p.shadow_near_plane, 0.0f, 10.0f);
    ImGui::SliderFloat("Shadow Far Plane", &p.shadow_far_plane, 1.0f, 100.0f);
    ImGui::SliderFloat("Camera Distance", &p.camera_distance, 0.0f, 10.0f);
    ImGui::SliderAngle("Camera FOV", &p.camera_fov, 0.0f, 90.0f);
    ImGui::SliderAngle("Camera Latitude", &p.camera_latitude, -90.0f, 90.0f);
    ImGui::SliderAngle("Camera Longitude", &p.camera_longitude, 0.0f, 360.0f);
}

auto TreeDemo::update(const UpdateDesc& desc) -> void {
    PIXScopedEvent(PIX_COLOR_DEFAULT, dx_name(NAME, "Update").data());

    auto& p = _parameters;

    // Update light angle.
    {
        p.light_longitude += desc.delta_time;
        if (p.light_longitude > PI * 2.0f) {
            p.light_longitude -= PI * 2.0f;
        }
    }

    // Light direction.
    auto light_direction = dir_from_lonlat(p.light_longitude, p.light_latitude);

    // Shadow pass - constants.
    Float4x4 light_transform;
    {
        auto projection = Float4x4::CreateOrthographic(
            p.light_projection_size,
            p.light_projection_size,
            p.shadow_near_plane,
            p.shadow_far_plane
        );
        auto eye = p.light_distance * light_direction;
        auto view = Float4x4::CreateLookAt(eye, Float3::Zero, Float3::Up);

        light_transform = view * projection;
    }

    // Main pass - constants.
    Float4x4 transform;
    {
        auto projection =
            Float4x4::CreatePerspectiveFieldOfView(p.camera_fov, desc.aspect_ratio, 0.1f, 100.0f);
        auto eye = p.camera_distance * dir_from_lonlat(p.camera_longitude, p.camera_latitude);
        auto at = Float3(0.0f, 3.0f, 0.0f);
        auto view = Float4x4::CreateLookAt(eye, at, Float3::Up);
        transform = view * projection;
    }

    // Debug.
    {
        _debug_draw.begin(desc.frame_index);
        _debug_draw.transform(transform);
        _debug_draw.axes();
        _debug_draw.line(Float3(), 16.0f * light_direction, COLOR_YELLOW);
        _debug_draw.end();
    }

    // Update constants.
    *_constants.ptr() = Constants {
        .transform = transform,
        .light_transform = light_transform,
        .light_direction = light_direction,
        .ambient_light = p.ambient_light,
    };
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
