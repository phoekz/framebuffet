set(NAME fb_kitchen)
set(SOURCES
    kitchen.cpp
    kitchen.hpp
    gpu/buffers.hpp
    gpu/commands.cpp
    gpu/commands.hpp
    gpu/debug.cpp
    gpu/debug.hpp
    gpu/descriptors.cpp
    gpu/descriptors.hpp
    gpu/device.cpp
    gpu/device.hpp
    gpu/formats.cpp
    gpu/formats.hpp
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
    kcn/brdf.hlsli
    kcn/core.hlsli
    kcn/debug_draw.cpp
    kcn/debug_draw.hlsli
    kcn/debug_draw.hpp
    kcn/gui.cpp
    kcn/gui.hlsli
    kcn/gui.hpp
    kcn/kcn.hpp
    kcn/multibuffer.hpp
    kcn/render_target.cpp
    kcn/render_target.hpp
    kcn/spd.hlsli
    utils/camera.cpp
    utils/camera.hpp
    utils/frame.cpp
    utils/frame.hpp
    win32/window.cpp
    win32/window.hpp
)
add_library(${NAME} STATIC ${SOURCES})
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_link_libraries(
    ${NAME}
    PUBLIC
    fb_common
    fb_baked
    d3d12.lib
    dxgi.lib
    dxguid.lib
    ${DEARIMGUI_LIBRARY}
    ${DXAGILITYSDK_LIBRARY}
    ${WINPIXEVENTRUNTIME_LIBRARY}
)
target_include_directories(
    ${NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${WINPIXEVENTRUNTIME_INCLUDE_DIR}
)
target_compile_definitions(
    ${NAME}
    PUBLIC
    FB_KITCHEN_PIX_GPU_CAPTURER_NAME="WinPixGpuCapturer.dll"
    FB_KITCHEN_PIX_GPU_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX/2409.23/WinPixGpuCapturer.dll"
    FB_KITCHEN_PIX_GPU_CAPTURE_FILE_NAME=L"framebuffet_gpu.wpix"
    FB_KITCHEN_PIX_TIMING_CAPTURER_NAME="WinPixTimingCapturer.dll"
    FB_KITCHEN_PIX_TIMING_CAPTURER_DLL_PATH="C:/Program Files/Microsoft PIX/2409.23/WinPixTimingCapturer.dll"
    FB_KITCHEN_PIX_TIMING_CAPTURE_FILE_NAME=L"framebuffet_timing.wpix"
)

fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${WINPIXEVENTRUNTIME_BIN_DIR}/WinPixEventRuntime.dll ${TARGET_DIRECTORY}
)
