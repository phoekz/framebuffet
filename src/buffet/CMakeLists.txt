set(NAME fb_buffet)
set(SOURCES
    buffet.cpp
    buffet.manifest
    baked/baked.cpp
    baked/baked.hpp
    demos/cards.cpp
    demos/cards.hpp
    demos/debug_draw.cpp
    demos/debug_draw.hpp
    demos/demos.hpp
    demos/gui.cpp
    demos/gui.hpp
    demos/render_targets.cpp
    demos/render_targets.hpp
    demos/cube/cube.cpp
    demos/cube/cube.hpp
    demos/tree/tree.cpp
    demos/tree/tree.hpp
    demos/rain/rain.cpp
    demos/rain/rain.hpp
    demos/anim/anim.cpp
    demos/anim/anim.hpp
    demos/fibers/fibers.cpp
    demos/fibers/fibers.hlsli
    demos/fibers/fibers.hpp
    demos/env/env.cpp
    demos/env/env.hpp
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
    gpu/textures.hpp
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
