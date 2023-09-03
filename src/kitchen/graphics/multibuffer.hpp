#pragma once

#include "../pch.hpp"
#include "../gpu/gpu.hpp"

namespace fb::graphics::multibuffer {

template<typename Buffer, uint BUFFER_COUNT>
class Multibuffer {
public:
    auto create(GpuDevice& device, uint element_count, std::string_view name) -> void {
        for (uint index = 0; auto& buffer : _buffers) {
            buffer.create(device, element_count, std::format("{} - {}", name, index));
            index++;
        }
    }

    auto buffer(uint frame_index) -> Buffer& { return _buffers[frame_index]; }
    auto buffer(uint frame_index) const -> const Buffer& { return _buffers[frame_index]; }
    auto buffer_count() const -> uint { return BUFFER_COUNT; }

private:
    std::array<Buffer, BUFFER_COUNT> _buffers;
};

} // namespace fb::graphics::multibuffer
