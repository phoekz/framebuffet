#include "techniques.hpp"

namespace fb::techniques {

auto create(Techniques& techs, const CreateDesc& desc) -> void {
    DebugScope debug("Techniques");

    techs.render_targets.create(
        desc.device,
        {
            .size = desc.device.swapchain().size(),
            .color_format = DXGI_FORMAT_R16G16B16A16_FLOAT,
            .clear_color = {0.3f, 0.6f, 0.0f, 1.0f},
            .sample_count = 4,
        }
    );

    techs.rect_texture_name = std::string("winter_evening");
    const auto rect_texture = desc.baked.stockcube.assets.winter_evening_hdr_texture();
    techs.rect_texture.create_and_transfer_baked(
        desc.device,
        rect_texture,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        debug.with_name("Rect Texture")
    );

    cfr::create(
        techs.cfr,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .rect_texture = techs.rect_texture.srv_descriptor(),
        }
    );
    lut::create(
        techs.lut,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    irr::create(
        techs.irr,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .cube_texture = techs.cfr.cube_texture.srv_descriptor(),
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    rad::create(
        techs.rad,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .cube_texture = techs.cfr.cube_texture.srv_descriptor(),
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    bg::create(
        techs.bg,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .rad_texture = techs.rad.div_texture.srv_descriptor(),
            .rad_texture_mip_count = techs.rad.div_texture.mip_count(),
        }
    );
    techs.debug_draw.create(desc.device, desc.baked.kitchen.shaders, techs.render_targets);
    model::create(
        techs.model,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .lut_texture = techs.lut.lut_texture.srv_descriptor(),
            .irr_texture = techs.irr.div_texture.srv_descriptor(),
            .rad_texture = techs.rad.div_texture.srv_descriptor(),
            .rad_texture_mip_count = techs.rad.div_texture.mip_count(),
        }
    );
    screen::create(
        techs.screen,
        {
            .render_targets = techs.render_targets,
            .baked = desc.baked,
            .device = desc.device,
            .rect_texture = techs.rect_texture.srv_descriptor(),
            .cube_texture = techs.cfr.cube_texture.alt_srv_descriptor(),
            .lut_texture = techs.lut.lut_texture.srv_descriptor(),
            .irr_texture = techs.irr.div_texture.alt_srv_descriptor(),
            .rad_texture = techs.rad.div_texture.alt_srv_descriptor(),
            .rad_texture_mip_count = techs.rad.div_texture.mip_count(),
        }
    );
    blit::create(
        techs.blit,
        {.render_targets = techs.render_targets, .baked = desc.baked, .device = desc.device}
    );
}

auto gui(Techniques& techs, const GuiDesc& desc) -> void {
#define wrap_gui(name)                                              \
    if (ImGui::TreeNodeEx(#name, ImGuiTreeNodeFlags_DefaultOpen)) { \
        name::gui(techs.name, desc);                                \
        ImGui::TreePop();                                           \
    }
    wrap_gui(cfr);
    wrap_gui(lut);
    wrap_gui(irr);
    wrap_gui(rad);
    wrap_gui(bg);
    wrap_gui(model);
    wrap_gui(screen);
    wrap_gui(blit);
#undef wrap_gui
}

auto update(Techniques& techs, const UpdateDesc& desc) -> void {
    cfr::update(techs.cfr, desc);
    lut::update(techs.lut, desc);
    irr::update(techs.irr, desc);
    rad::update(techs.rad, desc);
    bg::update(techs.bg, desc);
    techs.debug_draw.begin(desc.frame_index);
    techs.debug_draw.transform(desc.camera_view * desc.camera_projection);
    techs.debug_draw.scaled_axes(2.0f);
    techs.debug_draw.end();
    model::update(techs.model, desc);
    screen::update(techs.screen, desc);
    blit::update(techs.blit, desc);
}

auto main_pass(
    Techniques& techs,
    GpuDevice& device,
    RenderTargets& render_targets,
    GpuCommandList& cmd
) -> void {
    cfr::gpu_commands(techs.cfr, {.cmd = cmd});
    lut::gpu_commands(techs.lut, {.cmd = cmd});
    irr::gpu_commands(techs.irr, {.cmd = cmd});
    rad::gpu_commands(techs.rad, {.cmd = cmd});

    cmd.set_graphics();
    render_targets.set(cmd);
    bg::gpu_commands(techs.bg, {.cmd = cmd});
    techs.debug_draw.render(device, cmd);
    model::gpu_commands(techs.model, {.cmd = cmd});
    screen::gpu_commands(techs.screen, {.cmd = cmd});
}

auto compositing_pass(Techniques& techs, GpuDevice& device, GpuCommandList& cmd) -> void {
    blit::gpu_commands(techs.blit, {.device = device, .cmd = cmd});
}

} // namespace fb::techniques