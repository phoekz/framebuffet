#pragma once

#include "pch.hpp"

namespace fb {

inline constexpr std::string_view ARCHIVE_FILE_NAME = "fb_archive.bin"sv;

struct SerializingArchive final {
    explicit SerializingArchive(std::vector<std::byte>& buf)
        : buf(buf) {}
    std::vector<std::byte>& buf;
};

struct DeserializingArchive final {
    explicit DeserializingArchive(Span<const std::byte> buf)
        : buf(buf) {}
    auto fully_consumed() const -> bool { return buf.empty(); }
    Span<const std::byte> buf;
};

template<typename A>
concept Archive = std::is_same_v<A, SerializingArchive> || std::is_same_v<A, DeserializingArchive>;

template<typename T>
concept Archivable = std::is_trivially_copyable_v<T>;

template<Archive A, Archivable T>
inline void archive_trivial(A& arc, T& value) {
    if constexpr (std::is_same_v<A, SerializingArchive>) {
        const size_t size = sizeof(T);
        const auto offset = arc.buf.size();
        arc.buf.resize(offset + size);
        memcpy(arc.buf.data() + offset, &value, size);
    } else {
        const size_t size = sizeof(T);
        memcpy(&value, arc.buf.data(), size);
        arc.buf = arc.buf.last(arc.buf.size() - size);
    }
}

template<Archive A, Archivable T>
inline A& operator&(A& arc, T& value) {
    archive_trivial(arc, value);
    return arc;
}

} // namespace fb
