#pragma once

#include "../baked_types.hpp"

namespace fb::baked::kitchen {

class Assets {
public:
    auto load() -> void;

    auto imgui_font() const -> Copy;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_count) const -> std::span<const T> {
        return std::span((const T*)(_data.data() + offset), element_count);
    }

    std::vector<std::byte> _data;
};

class Shaders {
public:
    auto load() -> void;

    auto gui_draw_vs() const -> std::span<const std::byte>;
    auto gui_draw_ps() const -> std::span<const std::byte>;
    auto debug_draw_draw_vs() const -> std::span<const std::byte>;
    auto debug_draw_draw_ps() const -> std::span<const std::byte>;
    auto spd_downsample_cs() const -> std::span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::kitchen
