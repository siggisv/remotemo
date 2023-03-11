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

add_executable(init_no_conf_res src/init_helpers.cpp src/init_no_conf_res.cpp)
target_link_libraries(init_no_conf_res PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init_no_conf_res init_no_conf_res ${TEST_ARG})

add_executable(init_validate_conf_res
    src/init_helpers.cpp
    src/init_validate_conf_res.cpp
)
target_link_libraries(init_validate_conf_res PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init_validate_conf_res init_validate_conf_res ${TEST_ARG})

add_executable(init_setup src/init_helpers.cpp src/init_setup.cpp)
target_link_libraries(init_setup PRIVATE
    remotemo
    Catch2::Catch2WithMain
    trompeloeil::trompeloeil
)
add_test(test_init_setup init_setup ${TEST_ARG})
