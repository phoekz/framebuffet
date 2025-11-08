#pragma once

#include "../baked_types.hpp"

namespace fb::baked::griddle {

class Assets {
public:
    auto load() -> void;

private:
    template<typename T>
    auto transmuted_span(size_t offset, size_t element_count) const -> Span<const T> {
        return Span((const T*)(_data.data() + offset), element_count);
    }

    std::vector<std::byte> _data;
};

class Shaders {
public:
    auto load() -> void;

    auto griddle_vs() const -> Span<const std::byte>;
    auto griddle_ps() const -> Span<const std::byte>;

private:
    std::vector<std::byte> _data;
};

} // namespace fb::baked::griddle
