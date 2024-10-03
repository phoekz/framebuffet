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
    $<$<CONFIG:Debug>:/Zi> # Debug information format for Debug
    $<$<CONFIG:Release>:/Z7> # Debug information format for Release (/Zi does not work with precompiled headers)
    $<$<CONFIG:Release>:/Oi> # Enable intrinsic functions
    $<$<CONFIG:Release>:/Ot> # Favor fast code
    $<$<CONFIG:Release>:/GS-> # No buffer security checks
)
add_compile_definitions(
    _HAS_EXCEPTIONS=0
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
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
string(REGEX REPLACE "/Ob2" "/Ob3" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

# Helper functions.
function(fb_setup_visual_studio_directories current_source_dir sources)
    get_filename_component(LAST_DIR_NAME ${current_source_dir} NAME)
    source_group(TREE ${current_source_dir} PREFIX ${LAST_DIR_NAME} FILES ${sources})
    source_group(pch REGULAR_EXPRESSION "cmake_pch\.(hxx|cxx)")
endfunction()

# Definitions.
add_compile_definitions(
    FB_BAKER_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
    FB_BAKER_SOURCE_DIR_WIDE=L"${PROJECT_SOURCE_DIR}"
    FB_BAKER_BUFFET_OUTPUT_DIR="$<TARGET_FILE_DIR:fb_buffet>"
    FB_BAKER_STOCKCUBE_OUTPUT_DIR="$<TARGET_FILE_DIR:fb_stockcube>"
    FB_KITCHEN_PIX_GPU_CAPTURER_NAME="WinPixGpuCapturer.dll"
    FB_KITCHEN_PIX_GPU_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX/2409.23/WinPixGpuCapturer.dll"
    FB_KITCHEN_PIX_GPU_CAPTURE_FILE_NAME=L"framebuffet_gpu.wpix"
    FB_KITCHEN_PIX_TIMING_CAPTURER_NAME="WinPixTimingCapturer.dll"
    FB_KITCHEN_PIX_TIMING_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX/2409.23/WinPixTimingCapturer.dll"
    FB_KITCHEN_PIX_TIMING_CAPTURE_FILE_NAME=L"framebuffet_timing.wpix"
    FB_STOCKCUBE_INTERMEDIATE_DIR="${CMAKE_SOURCE_DIR}/src/assets/intermediate/stockcube"
)

# Libraries.
add_subdirectory(src/common)
add_subdirectory(src/baked)
add_subdirectory(src/kitchen)

# Executables.
add_subdirectory(src/baker)
add_subdirectory(src/stockcube)
add_subdirectory(src/buffet)
add_subdirectory(src/sandbox)
