#
# Configure fb_buffet.
#
set(NAME fb_buffet)
set(SOURCES
    baked/baked.cpp
    baked/baked.hpp
    buffet.cpp
    buffet.manifest
    demos/anim/anim.cpp
    demos/anim/anim.hlsli
    demos/anim/anim.hpp
    demos/cards.cpp
    demos/cards.hlsli
    demos/cards.hpp
    demos/crate/crate.cpp
    demos/crate/crate.hlsli
    demos/crate/crate.hpp
    demos/debug_draw.cpp
    demos/debug_draw.hlsli
    demos/debug_draw.hpp
    demos/demos.hpp
    demos/env/env.cpp
    demos/env/env.hlsli
    demos/env/env.hpp
    demos/fibers/fibers.cpp
    demos/fibers/fibers.hlsli
    demos/fibers/fibers.hpp
    demos/gui.cpp
    demos/gui.hlsli
    demos/gui.hpp
    demos/rain/rain.cpp
    demos/rain/rain.hlsli
    demos/rain/rain.hpp
    demos/render_targets.cpp
    demos/render_targets.hpp
    demos/tree/tree.cpp
    demos/tree/tree.hlsli
    demos/tree/tree.hpp
    gpu/buffers.hpp
    gpu/commands.cpp
    gpu/commands.hpp
    gpu/debug.cpp
    gpu/debug.hpp
    gpu/descriptors.cpp
    gpu/descriptors.hpp
    gpu/device.cpp
    gpu/device.hpp
    gpu/gpu.hpp
    gpu/hlsl_cpp.hlsli
    gpu/pipelines.cpp
    gpu/pipelines.hpp
    gpu/samplers.cpp
    gpu/samplers.hlsli
    gpu/samplers.hpp
    gpu/swapchain.cpp
    gpu/swapchain.hpp
    gpu/textures.hpp
    gpu/transfer.cpp
    gpu/transfer.hpp
    utils/frame.cpp
    utils/frame.hpp
    win32/window.cpp
    win32/window.hpp
)
add_executable(${NAME} ${SOURCES})
target_link_libraries(
    ${NAME} PRIVATE
    fb_common
    d3d12.lib
    dxgi.lib
    dxguid.lib
    ${DEARIMGUI_LIBRARY}
    ${DXAGILITYSDK_LIBRARY}
    ${DIRECTXTK12_LIBRARY}
    ${WINPIXEVENTRUNTIME_LIBRARY}
)
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${WINPIXEVENTRUNTIME_INCLUDE_DIR}
)
target_compile_definitions(
    ${NAME} PRIVATE
    FB_PIX_GPU_CAPTURER_NAME="WinPixGpuCapturer.dll"
    FB_PIX_GPU_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX Preview/2306.21-preview/WinPixGpuCapturer.dll"
    FB_PIX_TIMING_CAPTURER_NAME="WinPixTimingCapturer.dll"
    FB_PIX_TIMING_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX Preview/2306.21-preview/WinPixTimingCapturer.dll"
)

#
# Find PIX DLLs.
#
function(find_pix_dll OUT_DLL_FILE DLL_NAME SEARCH_PATHS)
    set(DLL_FILE "")
    find_library(${DLL_FILE}
        NAMES ${DLL_NAME}
        PATHS ${SEARCH_PATHS}
        NO_DEFAULT_PATH
        NO_CACHE
    )

    if(${DLL_NAME})
        message(STATUS "Found ${DLL_NAME} at ${DLL_FILE}")
    else()
        message(FATAL_ERROR "${DLL_NAME} not found")
    endif()

    set(${OUT_DLL_FILE} ${DLL_FILE} PARENT_SCOPE)
endfunction()

set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${WINPIXEVENTRUNTIME_BIN_DIR}/WinPixEventRuntime.dll ${TARGET_DIRECTORY}
)
