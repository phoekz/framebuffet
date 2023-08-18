#pragma once

#include "../pch.hpp"

namespace fb {

struct LeakTracker {
    ~LeakTracker();
    ComPtr<ID3D12DebugDevice2> debug_device;
};

auto dx_set_name(ID3D12Object* object, std::string_view name) -> void;
auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void;

template<typename... Args>
[[nodiscard]] auto dx_name(Args&&... args) -> std::string {
    static constexpr std::string_view DELIMITER = " - ";
    std::ostringstream oss;
    ((oss << args << DELIMITER), ...);
    std::string str = oss.str();
    str.erase(str.size() - DELIMITER.size());
    return str;
}

} // namespace fb
