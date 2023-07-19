include(FetchContent)

#
# Windows Implementation Library (WIL).
#

message(STATUS "Fetching WIL...")
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil
)
option(WIL_BUILD_PACKAGING OFF)
option(WIL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(wil)
set(WIL_INCLUDE_DIR ${WIL_SOURCE_DIR}/include CACHE INTERNAL "WIL Include Directory")

#
# D3D12 Memory Allocator.
#

message(STATUS "Fetching D3D12MemoryAllocator...")
set(_saved_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
FetchContent_Declare(
    d3d12memalloc
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
)
FetchContent_MakeAvailable(d3d12memalloc)
set(CMAKE_MESSAGE_LOG_LEVEL ${_saved_CMAKE_MESSAGE_LOG_LEVEL})
set(D3D12MEMALLOC_INCLUDE_DIR ${D3D12MemoryAllocator_SOURCE_DIR}/include CACHE INTERNAL "D3D12 Memory Allocator Include Directory")

#
# DirectX Agility SDK.
#

message(STATUS "Fetching DirectX Agility SDK...")
FetchContent_Declare(
    dxagilitysdk
    URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.d3d12.1.610.4.nupkg"
    URL_MD5 33bffd67d09a9008e49b35d4a8c4b8b5
)
FetchContent_MakeAvailable(dxagilitysdk)
set(DXAGILITYSDK_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxagilitysdk-src/build/native CACHE INTERNAL "DirectX Agility SDK Source Directory")
set(DXAGILITYSDK_INCLUDE_DIR ${DXAGILITYSDK_SOURCE_DIR}/include CACHE INTERNAL "DirectX Agility SDK Include Directory")
add_library(dxagilitysdk STATIC
    ${DXAGILITYSDK_SOURCE_DIR}/src/d3dx12/d3dx12_property_format_table.cpp
)
target_include_directories(dxagilitysdk PRIVATE
    ${DXAGILITYSDK_SOURCE_DIR}/include
    ${DXAGILITYSDK_SOURCE_DIR}/include/d3dx12
)
target_compile_definitions(dxagilitysdk PRIVATE WIN32)

#
# DirectX Shader Compiler.
#

message(STATUS "Fetching DirectX Shader Compiler...")
FetchContent_Declare(
    dxcompiler
    URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.dxc.1.7.2212.36.nupkg"
    URL_MD5 27673ca6b8e250e63ee5e741aeec1be4
)
FetchContent_MakeAvailable(dxcompiler)
set(DXCOMPILER_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxcompiler-src/build/native CACHE INTERNAL "DirectX Shader Compiler Source Directory")
set(DXCOMPILER_INCLUDE_DIR ${DXCOMPILER_SOURCE_DIR}/include CACHE INTERNAL "DirectX Shader Compiler Include Directory")
set(DXCOMPILER_LINK_DIR ${DXCOMPILER_SOURCE_DIR}/lib/x64 CACHE INTERNAL "DirectX Shader Compiler Link Directory")

#
# DirectXTK12.
#

message(STATUS "Fetching DirectXTK12...")
FetchContent_Declare(
    directxtk12
    URL "https://globalcdn.nuget.org/packages/directxtk12_desktop_2019.2023.4.28.1.nupkg"
    URL_MD5 09a48106645ce7296dbe7e8de56b877d
)
FetchContent_MakeAvailable(directxtk12)
set(DIRECTXTK12_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtk12-src)
file(GLOB_RECURSE DIRECTXTK12_INCLUDE_FILES ${DIRECTXTK12_SOURCE_DIR}/include/*.h ${DIRECTXTK12_SOURCE_DIR}/include/*.inl)
set(DIRECTXTK12_MODULE_INCLUDE_DIR ${DIRECTXTK12_SOURCE_DIR}/module-include/directxtk12)
file(MAKE_DIRECTORY ${DIRECTXTK12_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXTK12_INCLUDE_FILES} DESTINATION ${DIRECTXTK12_MODULE_INCLUDE_DIR})
set(DIRECTXTK12_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtk12-src CACHE INTERNAL "DirectXTK12 Source Directory")
set(DIRECTXTK12_INCLUDE_DIR ${DIRECTXTK12_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXTK12 Include Directory")
set(DIRECTXTK12_LINK_DIR ${DIRECTXTK12_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTK12 Link Directory")

#
# DirectXMesh.
#

message(STATUS "Fetching DirectXMesh...")
FetchContent_Declare(
    directxmesh
    URL "https://globalcdn.nuget.org/packages/directxmesh_desktop_win10.2023.4.28.1.nupkg"
    URL_MD5 18f92d2c5c432004e4c2cb48444defe6
)
FetchContent_MakeAvailable(directxmesh)
set(DIRECTXMESH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmesh-src)
file(GLOB_RECURSE DIRECTXMESH_INCLUDE_FILES ${DIRECTXMESH_SOURCE_DIR}/include/*.h ${DIRECTXMESH_SOURCE_DIR}/include/*.inl)
set(DIRECTXMESH_MODULE_INCLUDE_DIR ${DIRECTXMESH_SOURCE_DIR}/module-include/directxmesh)
file(MAKE_DIRECTORY ${DIRECTXMESH_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXMESH_INCLUDE_FILES} DESTINATION ${DIRECTXMESH_MODULE_INCLUDE_DIR})
set(DIRECTXMESH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmesh-src CACHE INTERNAL "DirectXMesh Source Directory")
set(DIRECTXMESH_INCLUDE_DIR ${DIRECTXMESH_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXMesh Include Directory")
set(DIRECTXMESH_LINK_DIR ${DIRECTXMESH_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXMesh Link Directory")

#
# DirectXTex.
#

message(STATUS "Fetching DirectXTex...")
FetchContent_Declare(
    directxtex
    URL "https://globalcdn.nuget.org/packages/directxtex_desktop_win10.2023.6.14.1.nupkg"
    URL_MD5 2f800e537e27a7f4a76ef00415b232e7
)
FetchContent_MakeAvailable(directxtex)
set(DIRECTXTEX_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtex-src)
file(GLOB_RECURSE DIRECTXTEX_INCLUDE_FILES ${DIRECTXTEX_SOURCE_DIR}/include/*.h ${DIRECTXTEX_SOURCE_DIR}/include/*.inl)
set(DIRECTXTEX_MODULE_INCLUDE_DIR ${DIRECTXTEX_SOURCE_DIR}/module-include/directxtex)
file(MAKE_DIRECTORY ${DIRECTXTEX_MODULE_INCLUDE_DIR})
file(COPY ${DIRECTXTEX_INCLUDE_FILES} DESTINATION ${DIRECTXTEX_MODULE_INCLUDE_DIR})
set(DIRECTXTEX_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxtex-src CACHE INTERNAL "DirectXTex Source Directory")
set(DIRECTXTEX_INCLUDE_DIR ${DIRECTXTEX_SOURCE_DIR}/module-include/ CACHE INTERNAL "DirectXTex Include Directory")
set(DIRECTXTEX_LINK_DIR ${DIRECTXTEX_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTex Link Directory")
