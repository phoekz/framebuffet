include(FetchContent)

#
# Dear ImGui.
#
message(STATUS "Fetching Dear ImGui...")
FetchContent_Declare(
    dearimgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG d4ddc46e7773e9a9b68f965d007968f35ca4e09a # 1.89.7
)
FetchContent_MakeAvailable(dearimgui)
set(DEARIMGUI_SOURCE_DIR ${dearimgui_SOURCE_DIR} CACHE INTERNAL "Dear ImGui Source Directory")
add_library(
    ext_dearimgui STATIC
    ${DEARIMGUI_SOURCE_DIR}/imgui.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_demo.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${DEARIMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${DEARIMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(
    ext_dearimgui
    PUBLIC
    ${DEARIMGUI_SOURCE_DIR}
    PRIVATE
    ${DEARIMGUI_SOURCE_DIR}/backends
)
file(GLOB_RECURSE DEARIMGUI_INCLUDE_FILES CONFIGURE_DEPENDS ${DEARIMGUI_SOURCE_DIR}/*.h ${DEARIMGUI_SOURCE_DIR}/*.inl)
target_sources(ext_dearimgui PRIVATE ${DEARIMGUI_INCLUDE_FILES})
set(DEARIMGUI_INCLUDE_DIR ${DEARIMGUI_SOURCE_DIR} CACHE INTERNAL "Dear ImGui Include Directory")
set(DEARIMGUI_LIBRARY ext_dearimgui CACHE INTERNAL "Dear ImGui Library")

#
# cgltf.
#
message(STATUS "Fetching cgltf...")
FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf
    GIT_TAG 271614ce5fa8bed6daf3bc824d12a3a652ebdb15 # Jun 15, 2023
)
FetchContent_MakeAvailable(cgltf)
set(CGLTF_SOURCE_DIR ${cgltf_SOURCE_DIR})
set(CGLTF_INCLUDE_DIR ${CGLTF_SOURCE_DIR} CACHE INTERNAL "cgltf Include Directory")

#
# stb.
#
message(STATUS "Fetching stb...")
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9 # Jan 29, 2023
)
FetchContent_MakeAvailable(stb)
set(STB_SOURCE_DIR ${stb_SOURCE_DIR})
set(STB_INCLUDE_DIR ${STB_SOURCE_DIR} CACHE INTERNAL "stb Include Directory")

#
# tinyexr
#
message(STATUS "Fetching tinyexr...")
FetchContent_Declare(
    tinyexr
    GIT_REPOSITORY https://github.com/syoyo/tinyexr
    GIT_TAG 6e8cac308cdf4d717078f3f37c4aa39bf3b356b4 # v1.0.7
)
option(TINYEXR_BUILD_SAMPLE OFF)
FetchContent_GetProperties(tinyexr)

if(NOT tinyexr_POPULATED)
    FetchContent_Populate(tinyexr)
    add_library(
        ext_tinyexr STATIC
        ${tinyexr_SOURCE_DIR}/tinyexr.cc
        ${tinyexr_SOURCE_DIR}/deps/miniz/miniz.c
    )
    target_include_directories(
        ext_tinyexr
        PUBLIC
        ${tinyexr_SOURCE_DIR}
        ${tinyexr_SOURCE_DIR}/deps/miniz
    )
endif()

set(TINYEXR_LIBRARY ext_tinyexr CACHE INTERNAL "tinyexr Library")

#
# fp16
#
message(STATUS "Fetching fp16...")
FetchContent_Declare(
    fp16
    GIT_REPOSITORY https://github.com/Maratyszcza/FP16.git
    GIT_TAG 0a92994d729ff76a58f692d3028ca1b64b145d91 # Mar 20, 2021
)
option(FP16_BUILD_TESTS OFF)
option(FP16_BUILD_BENCHMARKS OFF)
FetchContent_GetProperties(fp16)

if(NOT fp16_POPULATED)
    FetchContent_Populate(fp16)
    add_library(ext_fp16 INTERFACE)
    target_include_directories(
        ext_fp16
        INTERFACE
        ${fp16_SOURCE_DIR}/include
    )
endif()

set(FP16_LIBRARY ext_fp16 CACHE INTERNAL "fp16 Library")

#
# xxHash
#
message(STATUS "Fetching xxHash...")
FetchContent_Declare(
    xxhash
    GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
    GIT_TAG bbb27a5efb85b92a0486cf361a8635715a53f6ba # v0.8.2
)
FetchContent_GetProperties(xxhash)

if(NOT xxhash_POPULATED)
    FetchContent_Populate(xxhash)
    add_library(
        ext_xxhash STATIC
        ${xxhash_SOURCE_DIR}/xxhash.c
    )
    target_include_directories(
        ext_xxhash
        PUBLIC
        ${xxhash_SOURCE_DIR}
    )
endif()

set(XXHASH_LIBRARY ext_xxhash CACHE INTERNAL "xxhash Library")

#
# mikktspace
#
message(STATUS "Fetching mikktspace...")
FetchContent_Declare(
    mikktspace
    GIT_REPOSITORY https://github.com/mmikk/MikkTSpace.git
    GIT_TAG 3e895b49d05ea07e4c2133156cfa94369e19e409 # Mar 25, 2020
)
FetchContent_GetProperties(mikktspace)

if(NOT mikktspace_POPULATED)
    FetchContent_Populate(mikktspace)
    add_library(
        ext_mikktspace STATIC
        ${mikktspace_SOURCE_DIR}/mikktspace.c
    )
    target_include_directories(
        ext_mikktspace
        PUBLIC
        ${mikktspace_SOURCE_DIR}
    )
endif()

set(MIKKTSPACE_LIBRARY ext_mikktspace CACHE INTERNAL "mikktspace Library")

#
# Windows Implementation Library (WIL).
#
message(STATUS "Fetching WIL...")
FetchContent_Declare(
    wil
    GIT_REPOSITORY https://github.com/microsoft/wil
    GIT_TAG 55f373e164d3f092dd6c7a56e3de6f90c4c6f3dc # v1.0.230629.1
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
    GIT_TAG 8dc7b31598d5c89331475be1364f407ddf604f47 # Jul 11, 2023
)
FetchContent_GetProperties(d3d12memalloc)

if(NOT d3d12memalloc_POPULATED)
    FetchContent_Populate(d3d12memalloc)
    add_library(
        ext_d3d12memalloc STATIC
        ${d3d12memalloc_SOURCE_DIR}/src/D3D12MemAlloc.cpp
    )
    target_include_directories(
        ext_d3d12memalloc
        PUBLIC
        ${d3d12memalloc_SOURCE_DIR}/include
    )
endif()

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
add_library(ext_dxagilitysdk STATIC
    ${DXAGILITYSDK_SOURCE_DIR}/src/d3dx12/d3dx12_property_format_table.cpp
)
target_include_directories(
    ext_dxagilitysdk PUBLIC
    ${DXAGILITYSDK_SOURCE_DIR}/include
    ${DXAGILITYSDK_SOURCE_DIR}/include/d3dx12
)
file(GLOB_RECURSE DXAGILITYSDK_INCLUDE_FILES CONFIGURE_DEPENDS ${DXAGILITYSDK_SOURCE_DIR}/include/*.h)
target_sources(ext_dxagilitysdk PRIVATE ${DXAGILITYSDK_INCLUDE_FILES})
target_compile_definitions(ext_dxagilitysdk PRIVATE WIN32)
set(DXAGILITYSDK_LIBRARY ext_dxagilitysdk CACHE INTERNAL "DirectX Agility SDK Library")

#
# DirectX Shader Compiler.
#
message(STATUS "Fetching DirectX Shader Compiler...")
FetchContent_Declare(
    dxcompiler
    URL "https://globalcdn.nuget.org/packages/microsoft.direct3d.dxc.1.7.2308.12.nupkg"
    URL_MD5 59f6761d73ab36cda5152cb568b4bfd2
)
FetchContent_MakeAvailable(dxcompiler)
set(DXCOMPILER_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/dxcompiler-src/build/native CACHE INTERNAL "DirectX Shader Compiler Source Directory")
set(DXCOMPILER_INCLUDE_DIR ${DXCOMPILER_SOURCE_DIR}/include CACHE INTERNAL "DirectX Shader Compiler Include Directory")
set(DXCOMPILER_LINK_DIR ${DXCOMPILER_SOURCE_DIR}/lib/x64 CACHE INTERNAL "DirectX Shader Compiler Link Directory")

#
# DirectXMath.
#
message(STATUS "Fetching DirectXMath...")
FetchContent_Declare(
    directxmath
    URL "https://globalcdn.nuget.org/packages/directxmath.2022.12.12.1.nupkg"
    URL_MD5 b8fb4fda4eea8171627d0e080c4da062
)
FetchContent_MakeAvailable(directxmath)
set(DIRECTXMATH_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/directxmath-src CACHE INTERNAL "DirectXMath Source Directory")
set(DIRECTXMATH_INCLUDE_DIR ${DIRECTXMATH_SOURCE_DIR}/include CACHE INTERNAL "DirectXMath Include Directory")

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
set(DIRECTXTK12_DEBUG_LINK_DIR ${DIRECTXTK12_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXTK12 Debug Link Directory")
set(DIRECTXTK12_RELEASE_LINK_DIR ${DIRECTXTK12_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTK12 Release Link Directory")
set(DIRECTXTK12_LIBRARY
    $<$<CONFIG:Debug>:${DIRECTXTK12_DEBUG_LINK_DIR}/DirectXTK12.lib>
    $<$<CONFIG:Release>:${DIRECTXTK12_RELEASE_LINK_DIR}/DirectXTK12.lib>
    CACHE INTERNAL "DirectX Agility SDK Library")

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
set(DIRECTXMESH_DEBUG_LINK_DIR ${DIRECTXMESH_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXMesh Debug Link Directory")
set(DIRECTXMESH_RELEASE_LINK_DIR ${DIRECTXMESH_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXMesh Release Link Directory")

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
set(DIRECTXTEX_DEBUG_LINK_DIR ${DIRECTXTEX_SOURCE_DIR}/native/lib/x64/Debug CACHE INTERNAL "DirectXTex Debug Link Directory")
set(DIRECTXTEX_RELEASE_LINK_DIR ${DIRECTXTEX_SOURCE_DIR}/native/lib/x64/Release CACHE INTERNAL "DirectXTex Release Link Directory")

# WinPixEventRuntime.
#
message(STATUS "Fetching WinPixEventRuntime...")
FetchContent_Declare(
    winpixeventruntime
    URL "https://globalcdn.nuget.org/packages/winpixeventruntime.1.0.230302001.nupkg"
    URL_MD5 dcdc01fbef9e91d4bcdd13d3352d275e
)
FetchContent_MakeAvailable(winpixeventruntime)
set(WINPIXEVENTRUNTIME_SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/winpixeventruntime-src CACHE INTERNAL "WinPixEventRuntime Source Directory")
set(WINPIXEVENTRUNTIME_INCLUDE_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/Include CACHE INTERNAL "WinPixEventRuntime Include Directory")
set(WINPIXEVENTRUNTIME_BIN_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/bin/x64 CACHE INTERNAL "WinPixEventRuntime Binary Directory")
set(WINPIXEVENTRUNTIME_LINK_DIR ${WINPIXEVENTRUNTIME_SOURCE_DIR}/bin/x64 CACHE INTERNAL "WinPixEventRuntime Link Directory")
set(WINPIXEVENTRUNTIME_LIBRARY ${WINPIXEVENTRUNTIME_LINK_DIR}/WinPixEventRuntime.lib CACHE INTERNAL "WinPixEventRuntime Library")
