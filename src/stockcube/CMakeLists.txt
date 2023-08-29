set(NAME fb_stockcube)
set(SOURCES
    stockcube.cpp
    stockcube.manifest
)
add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} PRIVATE fb_kitchen)
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
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
