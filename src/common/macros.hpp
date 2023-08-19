#pragma once

#define FB_NO_COPY_MOVE(T)                 \
    T(const T&) = delete;                  \
    T(T&&) = delete;                       \
    auto operator=(const T&)->T& = delete; \
    auto operator=(T&&)->T& = delete;
