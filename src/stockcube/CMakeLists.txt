set(NAME fb_stockcube)
set(SOURCES
    stockcube.cpp
    stockcube.manifest
    techniques/common.hpp
    techniques/techniques.cpp
    techniques/techniques.hpp
    techniques/cfr/cfr.cpp
    techniques/cfr/cfr.hpp
    techniques/cfr/cfr.hlsli
    techniques/lut/lut.cpp
    techniques/lut/lut.hpp
    techniques/lut/lut.hlsli
    techniques/irr/irr.cpp
    techniques/irr/irr.hpp
    techniques/irr/irr.hlsli
    techniques/rad/rad.cpp
    techniques/rad/rad.hpp
    techniques/rad/rad.hlsli
    techniques/bg/bg.cpp
    techniques/bg/bg.hpp
    techniques/bg/bg.hlsli
    techniques/model/model.cpp
    techniques/model/model.hpp
    techniques/model/model.hlsli
    techniques/screen/screen.cpp
    techniques/screen/screen.hpp
    techniques/screen/screen.hlsli
    techniques/blit/blit.cpp
    techniques/blit/blit.hpp
    techniques/blit/blit.hlsli
)
add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} PRIVATE fb_kitchen ${NLOHMANN_JSON_LIBRARY})
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_compile_definitions(
    ${NAME} PRIVATE
    FB_STOCKCUBE_INTERMEDIATE_DIR="${CMAKE_SOURCE_DIR}/src/assets/intermediate/stockcube"
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
