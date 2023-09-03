#
# Configure fb_buffet.
#
set(NAME fb_buffet)
set(SOURCES
    buffet.cpp
    buffet.manifest
    demos/anim/anim.cpp
    demos/anim/anim.hlsli
    demos/anim/anim.hpp
    demos/cards/cards.cpp
    demos/cards/cards.hlsli
    demos/cards/cards.hpp
    demos/common.hpp
    demos/crate/crate.cpp
    demos/crate/crate.hlsli
    demos/crate/crate.hpp
    demos/demos.cpp
    demos/demos.hpp
    demos/env/env.cpp
    demos/env/env.hlsli
    demos/env/env.hpp
    demos/fibers/fibers.cpp
    demos/fibers/fibers.hlsli
    demos/fibers/fibers.hpp
    demos/rain/rain.cpp
    demos/rain/rain.hlsli
    demos/rain/rain.hpp
    demos/text/text.cpp
    demos/text/text.hlsli
    demos/text/text.hpp
    demos/tree/tree.cpp
    demos/tree/tree.hlsli
    demos/tree/tree.hpp
)
add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} PRIVATE fb_kitchen)
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NAME})
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
