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
target_include_directories(remotemo PUBLIC
    include
)
target_include_directories(remotemo PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)
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
    add_dependencies(hello_sample copy_resources)
endif()

if (REMOTEMO_BUILD_TESTS)
    add_subdirectory(tests)
endif()
