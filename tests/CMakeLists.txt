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
target_link_libraries(test_init PRIVATE
    remotemo
    SDL2::Main
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init test_init ${TEST_ARG})

add_executable(test_io
    src/test_io.cpp
)
target_link_libraries(test_io PRIVATE
    remotemo
    SDL2::Main
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
    Threads::Threads
)
add_test(test_io test_io ${TEST_ARG})
