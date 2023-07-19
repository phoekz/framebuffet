cmake_minimum_required(VERSION 3.26)

# Project.
project(framebuffet)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies.
add_subdirectory(external)

# Framebuffet.
add_executable(${PROJECT_NAME} src/main.cpp src/app.manifest)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${WIL_INCLUDE_DIR}
    ${D3D12MEMALLOC_INCLUDE_DIR}
    ${DXAGILITYSDK_INCLUDE_DIR}
    ${DXCOMPILER_INCLUDE_DIR}
    ${DIRECTXTK12_INCLUDE_DIR}
    ${DIRECTXMESH_INCLUDE_DIR}
    ${DIRECTXTEX_INCLUDE_DIR}
)
target_link_directories(${PROJECT_NAME} PRIVATE
    ${DXCOMPILER_LINK_DIR}
    ${DIRECTXTK12_LINK_DIR}
    ${DIRECTXMESH_LINK_DIR}
    ${DIRECTXTEX_LINK_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d12.lib
    dxgi.lib
    dxguid.lib
    D3D12MemoryAllocator
    dxagilitysdk
    dxcompiler.lib
    DirectXTK12.lib
    DirectXMesh.lib
    DirectXTex.lib
)
target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)

# Copy shaders to output directory.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/src/triangle.hlsl $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/
)

# Copy DLLs to output directory.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/D3D12Core.pdb $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXAGILITYSDK_SOURCE_DIR}/bin/x64/d3d12SDKLayers.pdb $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxcompiler.pdb $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    ${DXCOMPILER_SOURCE_DIR}/bin/x64/dxil.pdb $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
