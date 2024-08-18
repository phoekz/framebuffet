#pragma once

//
// Includes.
//

#include "../pch.hpp"
#include "debug_draw.hpp"
#include "gui.hpp"
#include "multibuffer.hpp"
#include "render_target.hpp"

//
// Aliases.
//

namespace fb {
template<typename Buffer, uint BufferCount>
using Multibuffer = graphics::multibuffer::Multibuffer<Buffer, BufferCount>;
using ColorAttachmentDesc = graphics::render_target::ColorAttachmentDesc;
using ColorAttachmentDescs = graphics::render_target::ColorAttachmentDescs;
using DepthStencilAttachmentDesc = graphics::render_target::DepthStencilAttachmentDesc;
using RenderTarget = graphics::render_target::RenderTarget;
using RenderTargetDesc = graphics::render_target::RenderTargetDesc;
using RenderTargetView = graphics::render_target::RenderTargetView;
using RenderTargetViewDesc = graphics::render_target::RenderTargetViewDesc;
using DebugDraw = graphics::debug_draw::DebugDraw;
using Gui = graphics::gui::Gui;
} // namespace fb
