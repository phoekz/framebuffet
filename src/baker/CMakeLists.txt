set(NAME fb_baker)
set(SOURCES
    baker.cpp
    assets/tasks.cpp
    assets/tasks.hpp
    assets/types.hpp
    formats/gltf.cpp
    formats/gltf.hpp
    formats/image.cpp
    formats/image.hpp
    formats/mikktspace.cpp
    formats/mikktspace.hpp
    outputs/outputs.cpp
    outputs/outputs.hpp
    outputs/templates/baked_cpp.hpp
    outputs/templates/baked_hpp.hpp
    outputs/templates/baked_types_hpp.hpp
    shaders/shaders.cpp
    shaders/shaders.hpp
    utils/names.hpp
)
add_executable(${NAME} ${SOURCES})
target_precompile_headers(${NAME} REUSE_FROM fb_common)
target_link_libraries(
    ${NAME} PRIVATE
    fb_common
    ${MIKKTSPACE_LIBRARY}
    ${STB_LIBRARY}
    ${CGLTF_LIBRARY}
    ${TINYEXR_LIBRARY}
    ${NLOHMANN_JSON_LIBRARY}
    ${TTF2MESH_LIBRARY}
    ${DXCOMPILER_LINK_DIR}/dxcompiler.lib
    d3d12.lib
)
target_include_directories(
    ${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${STB_INCLUDE_DIR}
    ${TINYEXR_INCLUDE_DIR}
    ${CGLTF_INCLUDE_DIR}
    ${DXCOMPILER_INCLUDE_DIR}
)

set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.pdb ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.dll ${TARGET_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.pdb ${TARGET_DIRECTORY}
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/build_print_binary_info.py ${TARGET_DIRECTORY}/${NAME}.exe
)
