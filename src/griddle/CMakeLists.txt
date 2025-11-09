#
# Configure fb_griddle.
#
set(NAME fb_griddle)
set(SOURCES
    griddle.cpp
    griddle.hlsli
    griddle.manifest)
add_executable(${NAME} ${SOURCES})
target_precompile_headers(${NAME} REUSE_FROM fb_common)
target_link_libraries(${NAME} PRIVATE fb_kitchen)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.pdb ${TARGET_DIRECTORY}
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/build_print_binary_info.py ${TARGET_DIRECTORY}/${NAME}.exe
)

if(FB_ENABLE_PIX)
    fb_pix_dependency(${NAME})
endif()
