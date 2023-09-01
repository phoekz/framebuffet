#pragma once

#include "../pch.hpp"
#include "common.hpp"
#include "anim/anim.hpp"
#include "crate/crate.hpp"
#include "env/env.hpp"
#include "fibers/fibers.hpp"
#include "rain/rain.hpp"
#include "tree/tree.hpp"
#include "cards/cards.hpp"

namespace fb::demos {

inline constexpr std::string_view NAME = "Buffet"sv;

struct Demos {
    anim::Demo anim;
    crate::Demo crate;
    env::Demo env;
    fibers::Demo fibers;
    rain::Demo rain;
    tree::Demo tree;
    cards::Demo cards;
};

struct CreateDesc {
    const Baked& baked;
    GpuDevice& device;
};

auto create(Demos& demos, const CreateDesc& desc) -> void;
auto gui(Demos& demos, const GuiDesc& desc) -> void;
auto update(Demos& demos, const UpdateDesc& desc) -> void;
auto transition_to_render_target(Demos& demos, const RenderDesc& desc) -> void;
auto clear_render_targets(Demos& demos, const RenderDesc& desc) -> void;
auto render_demos(Demos& demos, const RenderDesc& desc) -> void;
auto transition_to_resolve(Demos& demos, const RenderDesc& desc) -> void;
auto resolve_render_targets(Demos& demos, const RenderDesc& desc) -> void;
auto transition_to_srv(Demos& demos, const RenderDesc& desc) -> void;
auto render_compose(Demos& demos, const RenderDesc& desc) -> void;

template<Archive A>
auto archive(Demos& demos, A& arc) -> void {
    anim::archive(demos.anim, arc);
    crate::archive(demos.crate, arc);
    env::archive(demos.env, arc);
    fibers::archive(demos.fibers, arc);
    rain::archive(demos.rain, arc);
    tree::archive(demos.tree, arc);
    cards::archive(demos.cards, arc);
}

} // namespace fb::demos
