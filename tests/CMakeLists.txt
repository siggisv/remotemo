add_subdirectory(tools/Catch2)
add_subdirectory(tools/trompeloeil)

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

add_executable(init src/init.cpp)
target_link_libraries(init PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init init ${TEST_ARG})

add_executable(init_validate_res src/init_validate_res.cpp)
target_link_libraries(init_validate_res PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init_validate_res init_validate_res ${TEST_ARG})
