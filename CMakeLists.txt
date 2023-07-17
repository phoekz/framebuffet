cmake_minimum_required(VERSION 3.26)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(framebuffet VERSION 0.1.0)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wno-language-extension-token")

find_package(SDL2 CONFIG REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(d3d12-memory-allocator CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Guids)
target_link_libraries(${PROJECT_NAME} PRIVATE d3d12.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE dxgi.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE dxguid.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::d3d12-memory-allocator)
