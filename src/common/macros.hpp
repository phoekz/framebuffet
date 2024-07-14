#pragma once

#define FB_INLINE __forceinline

#define FB_NO_COPY(T)     \
                          \
private:                  \
    T(const T&) = delete; \
    auto operator=(const T&) -> T& = delete;

#define FB_NO_COPY_MOVE(T)                   \
                                             \
private:                                     \
    T(const T&) = delete;                    \
    T(T&&) = delete;                         \
    auto operator=(const T&) -> T& = delete; \
    auto operator=(T&&) -> T& = delete;
