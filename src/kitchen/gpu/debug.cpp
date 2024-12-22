#include "debug.hpp"

#include <sstream>

namespace fb {

auto dx_set_name(ID3D12Object* object, std::string_view name) -> void {
#if defined(_DEBUG)
    std::wstring wname = fb::to_wstr(name);
    object->SetName(wname.c_str());
#else
    (void)object;
    (void)name;
#endif
}

auto dx_set_name(const ComPtr<ID3D12Object>& object, std::string_view name) -> void {
    dx_set_name(object.get(), name);
}

struct DebugScopeContext {
    std::vector<std::string> stack;
};

static DebugScopeContext debug_scope_context;

DebugScope::DebugScope(std::string_view name) {
    debug_scope_context.stack.emplace_back(name);
}

DebugScope::~DebugScope() {
    FB_ASSERT(!debug_scope_context.stack.empty());
    debug_scope_context.stack.pop_back();
}

auto DebugScope::with_name(std::string_view name) -> std::string {
    static constexpr std::string_view DELIMITER = " - "sv;
    std::ostringstream oss;
    for (size_t i = 0; i < debug_scope_context.stack.size(); i++) {
        if (i != 0) {
            oss << DELIMITER;
        }
        oss << debug_scope_context.stack[i];
    }
    if (!debug_scope_context.stack.empty()) {
        oss << DELIMITER << name;
    }
    return oss.str();
}

} // namespace fb
