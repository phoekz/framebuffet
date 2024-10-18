set(NAME fb_tests)
set(SOURCES tests.cpp)
add_executable(${NAME} ${SOURCES})
target_precompile_headers(${NAME} REUSE_FROM fb_common)
target_link_libraries(${NAME} fb_kitchen ${CATCH2_LIBRARY})
target_include_directories(
    ${NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

fb_setup_visual_studio_directories(${CMAKE_CURRENT_SOURCE_DIR} "${SOURCES}")

set(TARGET_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
add_custom_command(
    TARGET ${NAME} POST_BUILD
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/build_print_binary_info.py ${TARGET_DIRECTORY}/${NAME}.exe
)
