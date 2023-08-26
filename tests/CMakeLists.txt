add_subdirectory(tools/Catch2)
add_subdirectory(tools/trompeloeil)

find_package(Threads REQUIRED)

option(REMOTEMO_VERBOSE_TESTS
    "Should the output from the tests themselve be more verbose?"
    OFF
)
IF (REMOTEMO_VERBOSE_TESTS)
    set(TEST_ARG ${TEST_ARG} "-s")
endif()

option(REMOTEMO_TESTS_COLOR 
    "Should the output from the tests themselve be in color?"
    ON
)
if (REMOTEMO_TESTS_COLOR)
    set(TEST_ARG ${TEST_ARG} "--colour-mode=ansi")
endif()

enable_testing()

add_executable(test_init
    src/test_init_helpers.cpp
    src/test_init_validate_conf_res.cpp
    src/test_init_setup.cpp
)
# It is enough for all the tests that the following runs for a single
# one of them:
add_custom_command(
    TARGET test_init POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:test_init>/res
    COMMENT "Copying resources into tests build directory"
)
if (TARGET SDL2::Main)
    target_link_libraries(test_init PRIVATE
        SDL2::Main
        SDL2::Image
    )
else()
    target_link_libraries(test_init PRIVATE
        SDL2::SDL2main
        SDL2_image::SDL2_image
    )
endif()
target_link_libraries(test_init PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init test_init ${TEST_ARG})

add_executable(test_io
    src/test_io.cpp
)
if (TARGET SDL2::Main)
    target_link_libraries(test_io PRIVATE
        SDL2::Main
        SDL2::Image
    )
else()
    target_link_libraries(test_io PRIVATE
        SDL2::SDL2main
        SDL2_image::SDL2_image
    )
endif()
target_link_libraries(test_io PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
    Threads::Threads
)
add_test(test_io test_io ${TEST_ARG})
