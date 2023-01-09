cmake_minimum_required(VERSION 3.16..3.25)

project(remotemo
    VERSION 0.0.1
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(REMOTEMO_BUILD_TESTS "Enable building the tests" OFF)
option(REMOTEMO_BUILD_SAMPLES "Enable building the samples" OFF)

set(is_gcc $<CXX_COMPILER_ID:GNU>)
set(is_clang $<CXX_COMPILER_ID:Clang,AppleClang>)
set(is_msvc $<CXX_COMPILER_ID:MSVC>)
set(debug_compile_options
    $<${is_gcc}:-Wall -Wextra -pedantic-errors>
    $<${is_clang}:-Wall -Wextra -pedantic-errors>
    $<${is_msvc}:/W4>
)
option(REMOTEMO_WERROR
    "Treat warning as errors (ignored if CMAKE_BUILD_TYPE is not Debug)"
    OFF
)
set(is_werror $<AND:$<CONFIG:Debug>,$<BOOL:${REMOTEMO_WERROR}>>)
set(werror_compile_option
    $<$<OR:${is_gcc},${is_clang}>:-Werror>
    $<${is_msvc}:/WX>
)
set(w_compile_options
    $<$<CONFIG:Debug>:${debug_compile_options}>
    $<${is_werror}:${werror_compile_option}>
)

find_package(SDL2 REQUIRED)

# TODO: This is working as it is on Linux Mint 20.3 but we might want to
# improve it so it should work on most systems:
# - Add variables to cache (empty by default) that when set by the user, will
#   be used for the [...]_INCLUDE_DIRS and the [...]_LIBRARIES variables.
# - Add FindSDL2[_module].cmake to a cmake directory and set its path:
#   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDL2_test_SOURCE_DIR}/cmake")
#   Can e.g. be found at https://github.com/tcbrindle/sdl2-cmake-scripts
# - Use find_packages (but set CMAKE_FIND_PACKAGE_PREFER_CONFIG to true so
#   that the config files provided by newer versions of the lib will be
#   prefered).
#   E.g.:
#   find_package(SDL2_image REQUIRED)
find_library(SDL2_IMAGE_LIBRARIES SDL2_image REQUIRED)
set(SDL2_IMAGES_INCLUDE_DIRS ${SDL2_INLCUDE_DIRS})

add_library(remotemo src/remotemo.cpp)
# How to prevent warnings in header files (when used in other projects):
# https://www.foonathan.net/2018/10/cmake-warnings
target_include_directories(remotemo PRIVATE
    include/
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)
target_include_directories(remotemo SYSTEM INTERFACE include/)

# Note. Since CMake 3.19, the following can be set in a preset
# file (i.e. CMakePresets.json)
target_compile_options(remotemo PRIVATE
    ${w_compile_options}
)
target_compile_definitions(remotemo PRIVATE $<$<CONFIG:Debug>:DEBUG>)

target_link_libraries(remotemo PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res
            ${CMAKE_CURRENT_BINARY_DIR}/res
    COMMENT "Copying resources into build directory"
)

IF (REMOTEMO_BUILD_SAMPLES)
    add_executable(hello_sample src/hello_sample.cpp)
    target_include_directories(hello_sample PUBLIC
        include
    )
    target_link_libraries(hello_sample PRIVATE
        remotemo
    )
    target_compile_options(hello_sample PRIVATE
        ${w_compile_options}
    )
    target_compile_definitions(hello_sample PRIVATE $<$<CONFIG:Debug>:DEBUG>)
    add_dependencies(hello_sample copy_resources)
endif()

if (REMOTEMO_BUILD_TESTS)
    add_subdirectory(tests)
endif()
