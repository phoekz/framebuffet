
#include "utils.hpp"

namespace fb {

//
// File I/O.
//

auto read_whole_file(std::string_view path) -> std::vector<std::byte> {
    wil::unique_hfile file(CreateFileA(
        path.data(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));
    FB_ASSERT_MSG(file.is_valid(), "Failed to open file.");

    LARGE_INTEGER file_size;
    FB_ASSERT_MSG(GetFileSizeEx(file.get(), &file_size), "Failed to get file size.");

    std::vector<std::byte> buffer(file_size.QuadPart);
    DWORD bytes_read;
    FB_ASSERT_MSG(
        ReadFile(file.get(), buffer.data(), (DWORD)buffer.size(), &bytes_read, nullptr),
        "Failed to read file.");

    return buffer;
}

//
// String conversions.
//

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

}  // namespace fb
