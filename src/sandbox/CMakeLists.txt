set(NAME fb_sandbox)
set(SOURCES sandbox.cpp)
add_executable(${NAME} ${SOURCES})
target_link_libraries(${NAME} fb_common)
target_include_directories(
    ${NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${STB_INCLUDE_DIR}
)

fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")
