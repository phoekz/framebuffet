cmake_minimum_required(VERSION 3.27)

# Globals.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)

# Project.
project(framebuffet)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies.
add_subdirectory(external)

# Global settings.
add_compile_options(
    /W4 /WX /MP /EHs-c- /GR-
    $<$<CONFIG:Debug>:/ZI>
    $<$<CONFIG:Release>:/Ob3>
    $<$<CONFIG:Release>:/Oi>
    $<$<CONFIG:Release>:/Ot>
    $<$<CONFIG:Release>:/Zi>
)
add_compile_definitions(
    _HAS_EXCEPTIONS=0
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)
add_link_options(
    /entry:mainCRTStartup
    /SUBSYSTEM:WINDOWS
    $<$<CONFIG:Release>:/DEBUG>
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
)
string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REGEX REPLACE "/Ob2" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS})

# Libraries.
add_subdirectory(src/common)

# Executables.
add_subdirectory(src/baker)
add_subdirectory(src/buffet)
add_subdirectory(src/sandbox)
