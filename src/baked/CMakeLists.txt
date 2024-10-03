set(NAME fb_baked)
set(SOURCES
    "baked_types.hpp"
    "buffet/baked.cpp"
    "buffet/baked.hpp"
    "kitchen/baked.cpp"
    "kitchen/baked.hpp"
    "stockcube/baked.cpp"
    "stockcube/baked.hpp"
)
add_library(${NAME} STATIC ${SOURCES})
target_precompile_headers(${NAME} REUSE_FROM fb_common)
target_link_libraries(
    ${NAME}
    PUBLIC
    fb_common
    ${DXAGILITYSDK_LIBRARY}
)
target_include_directories(
    ${NAME}
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")
