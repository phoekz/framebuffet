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
    demos.render_target_views = std::to_array({
        DemoRenderTargetViews(&demos.anim.render_target_view, &demos.anim.render_target_view),
        DemoRenderTargetViews(&demos.conras.render_target_view, &demos.conras.render_target_view),
        DemoRenderTargetViews(&demos.crate.render_target_view, &demos.crate.render_target_view),
        DemoRenderTargetViews(&demos.env.render_target_view, &demos.env.render_target_view),
        DemoRenderTargetViews(&demos.fibers.render_target_view, &demos.fibers.render_target_view),
        DemoRenderTargetViews(&demos.grass.render_target_view, &demos.grass.render_target_view),
        DemoRenderTargetViews(&demos.rain.render_target_view, &demos.rain.render_target_view),
        DemoRenderTargetViews(
            &demos.saber.scene.render_target_view,
            &demos.saber.blit.render_target_view
        ),
        DemoRenderTargetViews(&demos.text.render_target_view, &demos.text.render_target_view),
        DemoRenderTargetViews(&demos.tree.render_target_view, &demos.tree.render_target_view),
    });

    // Create cards.
#define X(lower_name, upper_name) \
    cards::CardDesc(lower_name::NAME, *demos.render_target_views[(uint)Demo::upper_name].post),
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
    FB_PERF_FUNC();

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
            default: FB_FATAL();
        }
    }
}

auto update(Demos& demos, const UpdateDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(name, _) name::update(demos.name, desc);
    DEMO_LIST(X);
#undef X

    cards::update(demos.cards, desc);
}

auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(_, name) \
    demos.render_target_views[(uint)Demo::name].pre->transition_to_render_target(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(lower_name, upper_name)                                               \
    {                                                                           \
        desc.cmd.pix_begin("Clear - %s", #upper_name);                          \
        demos.render_target_views[(uint)Demo::upper_name].pre->clear(desc.cmd); \
        desc.cmd.pix_end();                                                     \
    }
    DEMO_LIST(X);
#undef X
}

auto render_demos(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(name, _) name::render(demos.name, desc);
    DEMO_LIST(X);
#undef X
}

auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(_, name) \
    demos.render_target_views[(uint)Demo::name].post->transition_to_resolve(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(_, name) demos.render_target_views[(uint)Demo::name].post->resolve(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto transition_to_shader_resource(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
#define X(_, name) \
    demos.render_target_views[(uint)Demo::name].post->transition_to_shader_resource(desc.cmd);
    DEMO_LIST(X);
#undef X
}

auto render_compose(Demos& demos, const RenderDesc& desc) -> void {
    FB_PERF_FUNC();
    cards::render(demos.cards, desc);
}

} // namespace fb::demos
