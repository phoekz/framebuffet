#pragma once

#include <common/common.hpp>

namespace fb {

auto dx_set_name(ID3D12Object* object, std::string_view name) -> void;
auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void;

// Note: This is not thread-safe.
struct DebugScope {
    DebugScope(std::string_view name);
    ~DebugScope();
    auto with_name(std::string_view name) -> std::string;
};

} // namespace fb
