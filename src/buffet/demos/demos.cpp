#include "demos.hpp"

namespace fb::demos {

auto create(Demos& demos, const CreateDesc& desc) -> void {
    // Create demos.
#define X(name, _) name::create(demos.name, {.baked = desc.baked, .device = desc.device});
    DEMO_LIST(X);
#undef X

    // Configure render targets.
    //
    // Todo: this is only for the saber demo because it has two distinct phases
    // in its rendering. If we moved all transition/clear phases into demos,
    // this could be removed.
    demos.render_targets = std::to_array({
        DemoRenderTargets(&demos.anim.render_targets, &demos.anim.render_targets),
        DemoRenderTargets(&demos.conras.render_targets, &demos.conras.render_targets),
        DemoRenderTargets(&demos.crate.render_targets, &demos.crate.render_targets),
        DemoRenderTargets(&demos.env.render_targets, &demos.env.render_targets),
        DemoRenderTargets(&demos.fibers.render_targets, &demos.fibers.render_targets),
        DemoRenderTargets(&demos.grass.render_targets, &demos.grass.render_targets),
        DemoRenderTargets(&demos.rain.render_targets, &demos.rain.render_targets),
        DemoRenderTargets(&demos.saber.scene.render_targets, &demos.saber.blit.render_targets),
        DemoRenderTargets(&demos.text.render_targets, &demos.text.render_targets),
        DemoRenderTargets(&demos.tree.render_targets, &demos.tree.render_targets),
    });

    // Create cards.
#define X(lower_name, upper_name) \
    cards::CardDesc(lower_name::NAME, *demos.render_targets[(uint)Demo::upper_name].post),
    cards::create(
        demos.cards,
        {
            .baked = desc.baked,
            .device = desc.device,
            .cards = std::to_array({DEMO_LIST(X)}),
        }
    );
#undef X
}

auto gui(Demos& demos, const GuiDesc& desc) -> void {
    // Cards.
    {
        const auto flags = ImGuiTreeNodeFlags_None;
        ImGui::PushID(cards::NAME.data());
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
        switch ((Demo)hero_card) {
#define X(lower_name, upper_name) \
    case Demo::upper_name: lower_name::gui(demos.lower_name, desc); break;
            DEMO_LIST(X)
#undef X
            default: FB_FATAL(); break;
        }
    }
}

auto update(Demos& demos, const UpdateDesc& desc) -> void {
#define X(name, _) name::update(demos.name, desc);
    DEMO_LIST(X);
#undef X

    cards::update(demos.cards, desc);
}

auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(_, name) \
    demos.render_targets[(uint)Demo::name].pre->transition_to_render_target(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(_, name) demos.render_targets[(uint)Demo::name].pre->clear(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto render_demos(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(name, _) name::render(demos.name, desc);
    DEMO_LIST(X);
#undef X
}

auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(_, name) demos.render_targets[(uint)Demo::name].post->transition_to_resolve(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(_, name) demos.render_targets[(uint)Demo::name].post->resolve(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto transition_to_shader_resource(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
#define X(_, name) \
    demos.render_targets[(uint)Demo::name].post->transition_to_shader_resource(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto render_compose(Demos& demos, const RenderDesc& desc) -> void {
    ZoneScoped;
    cards::render(demos.cards, desc);
}

} // namespace fb::demos
