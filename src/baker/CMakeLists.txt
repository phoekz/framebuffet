set(NAME fb_baker)
set(SOURCES
    baker.cpp
    formats/stb.cpp
    formats/tinyexr.cpp
    formats/cgltf.cpp
    formats/image.cpp
    formats/image.hpp
    formats/exr.cpp
    formats/exr.hpp
    formats/gltf.cpp
    formats/gltf.hpp
    shaders/shaders.cpp
    shaders/shaders.hpp
)

add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} PRIVATE fb_common)
target_precompile_headers(${NAME} PRIVATE pch.hpp)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${STB_INCLUDE_DIR}
    ${TINYEXR_INCLUDE_DIR}
    ${CGLTF_INCLUDE_DIR}
    ${DXCOMPILER_INCLUDE_DIR}
)
target_link_libraries(
    ${NAME} PRIVATE
    ${TINYEXR_LIBRARY}
    ${DXCOMPILER_LINK_DIR}/dxcompiler.lib
    d3d12.lib
    ${DIRECTXTK12_LIBRARY}
)
target_compile_definitions(
    ${NAME} PRIVATE
    FRAMEBUFFET_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
    FRAMEBUFFET_SOURCE_DIR_WIDE=L"${PROJECT_SOURCE_DIR}"
    FRAMEBUFFET_OUTPUT_DIR="$<TARGET_FILE_DIR:fb_buffet>"
)

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${NAME})
set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.pdb ${TARGET_DIRECTORY}
)
