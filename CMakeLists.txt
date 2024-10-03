cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

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
# Note: Tracy does not work if /ZI is enabled. Therefore we must use /Zi.
add_compile_options(
    /W4 # Warning level 4
    /WX # Treat warnings as errors
    /MP # Multi-processor compilation
    /EHs-c- # No C++ exceptions
    /GR- # No RTTI
    /fp:except- # No floating point exceptions
    /arch:AVX2 # Enable AVX2
    $<$<CONFIG:Debug>:/Zi> # Debug information format
    $<$<CONFIG:Debug>:/Od> # Disable optimizations
    $<$<CONFIG:Release>:/O2> # Maximize speed
    $<$<CONFIG:Release>:/Ob3> # Inline function expansion
    $<$<CONFIG:Release>:/Oi> # Enable intrinsic functions
    $<$<CONFIG:Release>:/Ot> # Favor fast code
    $<$<CONFIG:Release>:/Zi> # Debug information format
    $<$<CONFIG:Release>:/GS-> # No buffer security checks
)
add_compile_definitions(
    _HAS_EXCEPTIONS=0
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Release>:NDEBUG>
)
add_link_options(
    /WX
    /entry:mainCRTStartup
    /SUBSYSTEM:WINDOWS
    $<$<CONFIG:Release>:/DEBUG>
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
)
string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REGEX REPLACE "/Ob2" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS})

# Helper functions.
function(fb_setup_visual_studio_directories current_source_dir sources)
    get_filename_component(LAST_DIR_NAME ${current_source_dir} NAME)
    source_group(TREE ${current_source_dir} PREFIX ${LAST_DIR_NAME} FILES ${sources})
    source_group(pch REGULAR_EXPRESSION "cmake_pch\.(hxx|cxx)")
endfunction()

# Libraries.
add_subdirectory(src/common)
add_subdirectory(src/baked)
add_subdirectory(src/kitchen)

# Executables.
add_subdirectory(src/baker)
add_subdirectory(src/stockcube)
add_subdirectory(src/buffet)
add_subdirectory(src/sandbox)
