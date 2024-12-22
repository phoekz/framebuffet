#pragma once

#include <common/common.hpp>
#include <kitchen/kitchen.hpp>
#include <baked/stockcube/baked.hpp>

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
    uint2 window_size;
    float aspect_ratio;
    float4x4 camera_view;
    float4x4 camera_projection;
    float3 camera_position;
    uint frame_index;
};

struct RenderDesc {
    GpuCommandList& cmd;
    GpuDevice& device;
    uint frame_index;
};

// Todo: this is a hack because we have not gotten into multi-queue
// synchronization yet. Can be used to delay an action for a couple of frames.
class Delayed {
public:
    static constexpr uint FRAME_DELAY = 3;

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
    Option<uint> _counter = std::nullopt;
};

struct OutputMetadata {
    DXGI_FORMAT format;
    uint unit_byte_count;
    uint width;
    uint height;
    uint depth;
    uint mip_count;
};

auto write_output_metadata_file(std::string_view path, const OutputMetadata& meta) -> void;

} // namespace fb::techniques
