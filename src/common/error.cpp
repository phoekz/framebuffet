#include "error.hpp"

#include <comdef.h>

namespace fb::error_detail {

auto hr_message(HRESULT hr) -> std::string {
    _com_error err(hr);
    return std::string {err.ErrorMessage()};
}

auto terminate() -> void {
    std::terminate();
}

} // namespace fb::error_detail
