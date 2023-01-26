add_subdirectory(tools/Catch2)
add_subdirectory(tools/trompeloeil)

add_executable(test_dummy src/dummy_test.cpp)
target_link_libraries(test_dummy PRIVATE
    Catch2::Catch2WithMain
)

add_executable(test_mock src/dummy_mock.cpp)
target_link_libraries(test_mock PRIVATE
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)

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
add_test(run_catch2_test test_dummy ${TEST_ARG})
add_test(run_trompeloeil_test test_mock ${TEST_ARG})

add_executable(remotemo_init src/remotemo_init.cpp)
target_link_libraries(remotemo_init PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_remotemo_init remotemo_init ${TEST_ARG})

