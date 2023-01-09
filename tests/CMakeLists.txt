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

option(REMOTEMO_TESTS_COLOR 
    "Should the output from the test themselve be in color?"
    ON
)
if (REMOTEMO_TESTS_COLOR)
    set(TEST_ARG "--colour-mode=ansi")
endif()

enable_testing()
add_test(run_catch2_test test_dummy ${TEST_ARG})
add_test(run_trompeloeil_test test_mock ${TEST_ARG})

