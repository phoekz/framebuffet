#pragma once

#include <common/common.hpp>

namespace fb {

class UniqueNames {
    FB_NO_COPY_MOVE(UniqueNames);

public:
    UniqueNames() = default;

    auto unique(std::string name) -> std::string {
        if (_names.contains(name)) {
            FB_LOG_ERROR("Duplicate name: {}", name);
            FB_FATAL();
        }
        _names.insert(name);
        return name;
    }

private:
    std::unordered_set<std::string> _names;
};

} // namespace fb
