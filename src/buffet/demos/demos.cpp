#include "demos.hpp"

namespace fb::demos {

auto create(Demos& demos, const CreateDesc& desc) -> void {
    anim::create(demos.anim, {.baked = desc.baked, .device = desc.device});
    crate::create(demos.crate, {.baked = desc.baked, .device = desc.device});
    env::create(demos.env, {.baked = desc.baked, .device = desc.device});
    fibers::create(demos.fibers, {.baked = desc.baked, .device = desc.device});
    rain::create(demos.rain, {.baked = desc.baked, .device = desc.device});
    text::create(demos.text, {.baked = desc.baked, .device = desc.device});
    tree::create(demos.tree, {.baked = desc.baked, .device = desc.device});
    cards::create(
        demos.cards,
        {
            .baked = desc.baked,
            .device = desc.device,
            .render_targets = std::to_array({
                std::ref(demos.anim.render_targets),
                std::ref(demos.crate.render_targets),
                std::ref(demos.env.render_targets),
                std::ref(demos.fibers.render_targets),
                std::ref(demos.rain.render_targets),
                std::ref(demos.tree.render_targets),
                std::ref(demos.text.render_targets),
            }),
        }
    );
}

auto gui(Demos& demos, const GuiDesc& desc) -> void {
#define gui_wrapper(name)                                                 \
    {                                                                     \
        const auto flags = ImGuiTreeNodeFlags_None;                       \
        ImGui::PushID(#name);                                             \
        if (ImGui::CollapsingHeader(name::NAME.data(), nullptr, flags)) { \
            name::gui(demos.name, desc);                                  \
        }                                                                 \
        ImGui::PopID();                                                   \
    }

    // Update cards first.
    gui_wrapper(cards);

    // Then update the rest.
    gui_wrapper(anim);
    gui_wrapper(crate);
    gui_wrapper(env);
    gui_wrapper(fibers);
    gui_wrapper(rain);
    gui_wrapper(text);
    gui_wrapper(tree);

#undef gui_wrapper
}

auto update(Demos& demos, const UpdateDesc& desc) -> void {
    anim::update(demos.anim, desc);
    crate::update(demos.crate, desc);
    env::update(demos.env, desc);
    fibers::update(demos.fibers, desc);
    rain::update(demos.rain, desc);
    text::update(demos.text, desc);
    tree::update(demos.tree, desc);
    cards::update(demos.cards, desc);
}

auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void {
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_render_target(cmd);
    demos.crate.render_targets.transition_to_render_target(cmd);
    demos.env.render_targets.transition_to_render_target(cmd);
    demos.fibers.render_targets.transition_to_render_target(cmd);
    demos.rain.render_targets.transition_to_render_target(cmd);
    demos.text.render_targets.transition_to_render_target(cmd);
    demos.tree.render_targets.transition_to_render_target(cmd);
}

auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    auto& cmd = desc.cmd;
    demos.anim.render_targets.clear(cmd);
    demos.crate.render_targets.clear(cmd);
    demos.env.render_targets.clear(cmd);
    demos.fibers.render_targets.clear(cmd);
    demos.rain.render_targets.clear(cmd);
    demos.text.render_targets.clear(cmd);
    demos.tree.render_targets.clear(cmd);
}

auto render_demos(Demos& demos, const RenderDesc& desc) -> void {
    anim::render(demos.anim, desc);
    crate::render(demos.crate, desc);
    env::render(demos.env, desc);
    fibers::render(demos.fibers, desc);
    rain::render(demos.rain, desc);
    text::render(demos.text, desc);
    tree::render(demos.tree, desc);
}

auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void {
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_resolve(cmd);
    demos.crate.render_targets.transition_to_resolve(cmd);
    demos.env.render_targets.transition_to_resolve(cmd);
    demos.fibers.render_targets.transition_to_resolve(cmd);
    demos.rain.render_targets.transition_to_resolve(cmd);
    demos.text.render_targets.transition_to_resolve(cmd);
    demos.tree.render_targets.transition_to_resolve(cmd);
}

auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void {
    auto& cmd = desc.cmd;
    demos.anim.render_targets.resolve(cmd);
    demos.crate.render_targets.resolve(cmd);
    demos.env.render_targets.resolve(cmd);
    demos.fibers.render_targets.resolve(cmd);
    demos.rain.render_targets.resolve(cmd);
    demos.text.render_targets.resolve(cmd);
    demos.tree.render_targets.resolve(cmd);
}

auto transition_to_shader_resource(Demos& demos, const RenderDesc& desc) -> void {
    auto& cmd = desc.cmd;
    demos.anim.render_targets.transition_to_shader_resource(cmd);
    demos.crate.render_targets.transition_to_shader_resource(cmd);
    demos.env.render_targets.transition_to_shader_resource(cmd);
    demos.fibers.render_targets.transition_to_shader_resource(cmd);
    demos.rain.render_targets.transition_to_shader_resource(cmd);
    demos.text.render_targets.transition_to_shader_resource(cmd);
    demos.tree.render_targets.transition_to_shader_resource(cmd);
}

auto render_compose(Demos& demos, const RenderDesc& desc) -> void {
    cards::render(demos.cards, desc);
}

} // namespace fb::demos
