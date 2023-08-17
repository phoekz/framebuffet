#pragma once

#include "pch.hpp"

namespace fb {

// Enables the following pattern:
// ```
// using Enum = std::variant<int, float, std::string>;
// Enum e = ...;
// std::visit(
//     overloaded {
//         [](int i) { ... },
//         [](float f) { ... },
//         [](std::string s) { ... },
//     },
//     e);
// ```
// Read more: https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded: Ts... {
    using Ts::operator()...;
};

}  // namespace fb
