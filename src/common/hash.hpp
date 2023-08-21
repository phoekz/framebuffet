#pragma once

#include "pch.hpp"

namespace fb {

struct Hash128 {
    uint64_t low;
    uint64_t high;
};

auto hash128(std::span<const std::byte> data) -> Hash128;

} // namespace fb

template<>
struct std::formatter<fb::Hash128>: std::formatter<char> {
    template<class FormatContext>
    auto format(fb::Hash128 v, FormatContext& fc) const {
        return std::format_to(fc.out(), "{:016x}{:016x}", v.high, v.low);
    }
};
