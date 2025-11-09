#pragma once

#include "../baked_types.hpp"

namespace fb::baked::kitchen {

class Assets {
public:
    auto load() -> void;

    auto imgui_font() const -> Copy;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_count) const -> Span<const T> {
        return Span<const T>((const T*)(_file.bytes() + offset), element_count);
    }

    FileBuffer _file;
};

class Shaders {
public:
    auto load() -> void;

    auto gui_draw_vs() const -> Span<const std::byte>;
    auto gui_draw_ps() const -> Span<const std::byte>;
    auto debug_draw_draw_vs() const -> Span<const std::byte>;
    auto debug_draw_draw_ps() const -> Span<const std::byte>;
    auto spd_downsample_cs() const -> Span<const std::byte>;

private:
    FileBuffer _file;
};

} // namespace fb::baked::kitchen
