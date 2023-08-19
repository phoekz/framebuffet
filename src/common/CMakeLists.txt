set(NAME fb_common)
set(SOURCES
    archive.hpp
    com.hpp
    common.cpp
    common.hpp
    error.cpp
    error.hpp
    file.cpp
    file.hpp
    log.cpp
    log.hpp
    macros.hpp
    math.hpp
    pcg.cpp
    pcg.hpp
    string.cpp
    string.hpp
    template_helpers.hpp
    time.cpp
    time.hpp
)
add_library(${NAME} STATIC ${SOURCES})
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME}
    PUBLIC
    ${WIL_INCLUDE_DIR}
    ${DIRECTXMATH_INCLUDE_DIR}
    ${DIRECTXTK12_INCLUDE_DIR}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
