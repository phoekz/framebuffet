#pragma once

#include "../pch.hpp"
#include <kitchen/kitchen.hpp>
#include <baked/stockcube/baked.hpp>

using RenderTargets = fb::graphics::render_targets::RenderTargets;
using DebugDraw = fb::graphics::debug_draw::DebugDraw;
using Gui = fb::graphics::gui::Gui;

namespace fb::techniques {

struct Baked {
    struct {
        baked::kitchen::Assets assets;
        baked::kitchen::Shaders shaders;
    } kitchen;
    struct {
        baked::stockcube::Assets assets;
        baked::stockcube::Shaders shaders;
    } stockcube;
};

struct GuiDesc {};

struct UpdateDesc {
    Uint2 window_size;
    float aspect_ratio;
    Float4x4 camera_view;
    Float4x4 camera_projection;
    Float3 camera_position;
    uint32_t frame_index;
};

// Todo: this is a hack because we have not gotten into multi-queue
// synchronization yet. Can be used to delay an action for a couple of frames.
class Delayed {
public:
    static constexpr uint32_t FRAME_DELAY = 3;

    auto init() -> void { _initiated = true; }

    auto initiated() const -> bool { return _initiated; }

    auto set_pending() -> void {
        _initiated = false;
        _counter = 0;
    }

    template<typename Callback>
    auto until(Callback callback) -> void {
        if (!_counter.has_value()) {
            return;
        }

        if (_counter.value() == FRAME_DELAY) {
            callback();
            _counter = std::nullopt;
        } else {
            _counter.value()++;
        }
    }

private:
    bool _initiated = false;
    std::optional<uint32_t> _counter = std::nullopt;
};

struct OutputMetadata {
    DXGI_FORMAT format;
    uint32_t unit_byte_count;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_count;
};

inline auto write_output_metadata_file(std::string_view path, const OutputMetadata& meta) -> void {
    const ord_json json = {
        {"format", meta.format},
        {"unit_byte_count", meta.unit_byte_count},
        {"width", meta.width},
        {"height", meta.height},
        {"depth", meta.depth},
        {"mip_count", meta.mip_count},
    };
    const auto json_str = json.dump(4);
    const auto json_bytes = std::as_bytes(std::span(json_str));
    write_whole_file(path, json_bytes);
}

} // namespace fb::techniques
