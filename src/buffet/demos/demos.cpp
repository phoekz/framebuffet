#include "demos.hpp"

namespace fb::demos {

auto create(Demos& demos, const CreateDesc& desc) -> void {
    // Create demos.
    anim::create(demos.anim, {.baked = desc.baked, .device = desc.device});
    conras::create(demos.conras, {.baked = desc.baked, .device = desc.device});
    crate::create(demos.crate, {.baked = desc.baked, .device = desc.device});
    env::create(demos.env, {.baked = desc.baked, .device = desc.device});
    fibers::create(demos.fibers, {.baked = desc.baked, .device = desc.device});
    grass::create(demos.grass, {.baked = desc.baked, .device = desc.device});
    rain::create(demos.rain, {.baked = desc.baked, .device = desc.device});
    saber::create(demos.saber, {.baked = desc.baked, .device = desc.device});
    text::create(demos.text, {.baked = desc.baked, .device = desc.device});
    tree::create(demos.tree, {.baked = desc.baked, .device = desc.device});

    // Create cards.
    cards::create(
        demos.cards,
        {
            .baked = desc.baked,
            .device = desc.device,
            .cards = std::to_array({
                cards::CardDesc(anim::NAME, std::cref(demos.anim.render_targets)),
                cards::CardDesc(conras::NAME, std::cref(demos.conras.render_targets)),
                cards::CardDesc(crate::NAME, std::cref(demos.crate.render_targets)),
                cards::CardDesc(env::NAME, std::cref(demos.env.render_targets)),
                cards::CardDesc(fibers::NAME, std::cref(demos.fibers.render_targets)),
                cards::CardDesc(grass::NAME, std::cref(demos.grass.render_targets)),
                cards::CardDesc(rain::NAME, std::cref(demos.rain.render_targets)),
                cards::CardDesc(saber::NAME, std::cref(demos.saber.blit.render_targets)),
                cards::CardDesc(tree::NAME, std::cref(demos.tree.render_targets)),
                cards::CardDesc(text::NAME, std::cref(demos.text.render_targets)),
            }),
        }
    );
}

auto gui(Demos& demos, const GuiDesc& desc) -> void {
    // Cards.
    {
        const auto flags = ImGuiTreeNodeFlags_DefaultOpen;
        ImGui::PushID("Cards");
        if (ImGui::CollapsingHeader(cards::NAME.data(), nullptr, flags)) {
            cards::gui(demos.cards, desc);
        }
        ImGui::PopID();
    }

    ImGui::Separator();

    // Hero card.
    const auto hero_card = demos.cards.parameters.card_indirect_indices.back();
    const auto hero_name = demos.cards.card_names[hero_card];
    if (ImGui::CollapsingHeader(hero_name.data(), nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
        switch (hero_card) {
            case 0: anim::gui(demos.anim, desc); break;
            case 1: conras::gui(demos.conras, desc); break;
            case 2: crate::gui(demos.crate, desc); break;
            case 3: env::gui(demos.env, desc); break;
            case 4: fibers::gui(demos.fibers, desc); break;
            case 5: grass::gui(demos.grass, desc); break;
            case 6: rain::gui(demos.rain, desc); break;
            case 7: saber::gui(demos.saber, desc); break;
            case 8: text::gui(demos.text, desc); break;
            case 9: tree::gui(demos.tree, desc); break;
            default: FB_FATAL(); break;
        }
    }
}

auto update(Demos& demos, const UpdateDesc& desc) -> void {
    anim::update(demos.anim, desc);
    conras::update(demos.conras, desc);
    crate::update(demos.crate, desc);
    env::update(demos.env, desc);
    fibers::update(demos.fibers, desc);
    grass::update(demos.grass, desc);
    rain::update(demos.rain, desc);
    saber::update(demos.saber, desc);
    text::update(demos.text, desc);
    tree::update(demos.tree, desc);
    cards::update(demos.cards, desc);
}

auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_render_target(cmd);
    demos.conras.render_targets.transition_to_render_target(cmd);
    demos.crate.render_targets.transition_to_render_target(cmd);
    demos.env.render_targets.transition_to_render_target(cmd);
    demos.fibers.render_targets.transition_to_render_target(cmd);
    demos.grass.render_targets.transition_to_render_target(cmd);
    demos.rain.render_targets.transition_to_render_target(cmd);
    demos.saber.scene.render_targets.transition_to_render_target(cmd);
    demos.text.render_targets.transition_to_render_target(cmd);
    demos.tree.render_targets.transition_to_render_target(cmd);
}

auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& cmd = desc.cmd;
    demos.anim.render_targets.clear(cmd);
    demos.conras.render_targets.clear(cmd);
    demos.crate.render_targets.clear(cmd);
    demos.env.render_targets.clear(cmd);
    demos.fibers.render_targets.clear(cmd);
    demos.grass.render_targets.clear(cmd);
    demos.rain.render_targets.clear(cmd);
    demos.saber.scene.render_targets.clear(cmd);
    demos.text.render_targets.clear(cmd);
    demos.tree.render_targets.clear(cmd);
}

auto render_demos(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    anim::render(demos.anim, desc);
    conras::render(demos.conras, desc);
    crate::render(demos.crate, desc);
    env::render(demos.env, desc);
    fibers::render(demos.fibers, desc);
    grass::render(demos.grass, desc);
    rain::render(demos.rain, desc);
    saber::render(demos.saber, desc);
    text::render(demos.text, desc);
    tree::render(demos.tree, desc);
}

auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_resolve(cmd);
    demos.conras.render_targets.transition_to_resolve(cmd);
    demos.crate.render_targets.transition_to_resolve(cmd);
    demos.env.render_targets.transition_to_resolve(cmd);
    demos.fibers.render_targets.transition_to_resolve(cmd);
    demos.grass.render_targets.transition_to_resolve(cmd);
    demos.rain.render_targets.transition_to_resolve(cmd);
    demos.text.render_targets.transition_to_resolve(cmd);
    demos.tree.render_targets.transition_to_resolve(cmd);
}

auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& cmd = desc.cmd;
    demos.anim.render_targets.resolve(cmd);
    demos.conras.render_targets.resolve(cmd);
    demos.crate.render_targets.resolve(cmd);
    demos.env.render_targets.resolve(cmd);
    demos.fibers.render_targets.resolve(cmd);
    demos.grass.render_targets.resolve(cmd);
    demos.rain.render_targets.resolve(cmd);
    demos.text.render_targets.resolve(cmd);
    demos.tree.render_targets.resolve(cmd);
}

auto transition_to_shader_resource(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_shader_resource(cmd);
    demos.conras.render_targets.transition_to_shader_resource(cmd);
    demos.crate.render_targets.transition_to_shader_resource(cmd);
    demos.env.render_targets.transition_to_shader_resource(cmd);
    demos.fibers.render_targets.transition_to_shader_resource(cmd);
    demos.grass.render_targets.transition_to_shader_resource(cmd);
    demos.rain.render_targets.transition_to_shader_resource(cmd);
    demos.saber.blit.render_targets.transition_to_shader_resource(cmd);
    demos.text.render_targets.transition_to_shader_resource(cmd);
    demos.tree.render_targets.transition_to_shader_resource(cmd);
}

auto render_compose(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    cards::render(demos.cards, desc);
}

} // namespace fb::demos
