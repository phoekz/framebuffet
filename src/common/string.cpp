#include "string.hpp"

namespace fb {

auto from_wstr(std::wstring_view wstr) -> std::string {
    const int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, &str[0], size, nullptr, nullptr);
    str.pop_back();
    return str;
}

auto to_wstr(std::string_view str) -> std::wstring {
    const int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, &wstr[0], size);
    wstr.pop_back();
    return wstr;
}

} // namespace fb
