#include "techniques.hpp"

namespace fb::techniques {

auto create(Techniques& techs, const CreateDesc& desc) -> void {
    DebugScope debug("Techniques");

    techs.render_target.create(
        desc.device,
        {
            .size = desc.device.swapchain().size(),
            .sample_count = 4,
            .colors = {KcnColorAttachmentDesc {
                .format = DXGI_FORMAT_R16G16B16A16_FLOAT,
                .clear_color = {0.3f, 0.6f, 0.0f, 1.0f},
            }},
            .depth_stencil = {
                .format = DXGI_FORMAT_D32_FLOAT,
                .clear_depth = 1.0f,
                .clear_stencil = 0,
            },
        }
    );
    techs.render_target_view.create(techs.render_target.view_desc());

#if 0
    techs.rect_texture_name = std::string("farm_field");
    const auto rect_texture = desc.baked.stockcube.assets.farm_field_hdr_texture();
#elif 0
    techs.rect_texture_name = std::string("industrial_sunset_02_puresky");
    const auto rect_texture =
        desc.baked.stockcube.assets.industrial_sunset_02_puresky_hdr_texture();
#elif 0
    techs.rect_texture_name = std::string("winter_evening");
    const auto rect_texture = desc.baked.stockcube.assets.winter_evening_hdr_texture();
#else
    techs.rect_texture_name = std::string("shanghai_bund");
    const auto rect_texture = desc.baked.stockcube.assets.shanghai_bund_hdr_texture();
#endif

    techs.rect_texture.create_and_transfer_baked(
        desc.device,
        rect_texture,
        D3D12_BARRIER_SYNC_PIXEL_SHADING,
        D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
        D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
        debug.with_name("Rect Texture")
    );

    cfr::create(
        techs.cfr,
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
            .rect_texture = techs.rect_texture.srv_descriptor(),
        }
    );
    lut::create(
        techs.lut,
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    irr::create(
        techs.irr,
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
            .cube_texture = techs.cfr.cube_texture.srv_descriptor(),
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    rad::create(
        techs.rad,
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
            .cube_texture = techs.cfr.cube_texture.srv_descriptor(),
            .texture_name = std::string_view(techs.rect_texture_name),
        }
    );
    bg::create(
        techs.bg,
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
            .rad_texture = techs.rad.div_texture.srv_descriptor(),
            .rad_texture_mip_count = techs.rad.div_texture.mip_count(),
        }
    );
    techs.debug_draw.create(desc.device, desc.baked.kitchen.shaders, techs.render_target_view);
    model::create(
        techs.model,
        {
            .render_target_view = techs.render_target_view,
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
            .render_target_view = techs.render_target_view,
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
        {
            .render_target_view = techs.render_target_view,
            .baked = desc.baked,
            .device = desc.device,
        }
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

auto render_main(Techniques& techs, KcnRenderTargetView& render_target_view, const RenderDesc& desc)
    -> void {
    cfr::render(techs.cfr, desc);
    lut::render(techs.lut, desc);
    irr::render(techs.irr, desc);
    rad::render(techs.rad, desc);

    desc.cmd.graphics_scope([&techs, &render_target_view](GpuGraphicsCommandList& cmd) {
        render_target_view.set_graphics(cmd);
        techs.debug_draw.render(cmd);
    });
    bg::render(techs.bg, desc);
    model::render(techs.model, desc);
    screen::render(techs.screen, desc);
}

auto render_compositing(Techniques& techs, const RenderDesc& desc) -> void {
    blit::render(techs.blit, desc);
}

} // namespace fb::techniques
