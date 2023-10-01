cmake_minimum_required(VERSION 3.16..3.25)

project(remotemo
    VERSION 0.1.0
    LANGUAGES CXX
)

# Add pre-release label to version number and define in header file
set(remotemo_PRE_RELEASE_LABEL "rc.1")
if(NOT remotemo_PRE_RELEASE_LABEL STREQUAL "")
    set(remotemo_VERSION
        ${remotemo_VERSION}-${remotemo_PRE_RELEASE_LABEL}
    )
endif()
configure_file(
    "${PROJECT_SOURCE_DIR}/src/version.hpp.in"
    "${PROJECT_BINARY_DIR}/src/version.hpp"
)

# C++ 17, without gnu-extension, is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings and warnings-as-error
option(REMOTEMO_WARNINGS
    "Enable compiler warning flags"
    OFF
)
option(REMOTEMO_WERROR
    "Treat warning as errors (Note: MSVC warns about stuff in SDL2, etc)"
    OFF
)

# Enable building various extras:
option(REMOTEMO_BUILD_TESTS "Enable building the tests" OFF)
option(REMOTEMO_BUILD_SAMPLES "Enable building the samples" OFF)

# Enable generation of file needed for the clangd language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL
    "Enable generation of 'compile_commands.json'"
)

# Options enabling and setting document generation with Doxygen:
option(REMOTEMO_BUILD_DOCS
    "Enable generating documentation (requires Doxygen and '--target docs')"
    OFF
)
set(REMOTEMO_DOXYGEN_EXTRACT_ALL YES CACHE STRING
    "Undocumented members treated as documented (disables Doxygen warnings)"
)
set_property(CACHE REMOTEMO_DOXYGEN_EXTRACT_ALL PROPERTY STRINGS YES NO)

# Generate documents (with Doxygen)
if(REMOTEMO_BUILD_DOCS)
    include(cmake/Doxygen.cmake)
endif(REMOTEMO_BUILD_DOCS)

# Set compiler options enabling warnings and warnings-as-error:
set(is_gcc $<CXX_COMPILER_ID:GNU>)
set(is_clang $<CXX_COMPILER_ID:Clang,AppleClang>)
set(is_msvc $<CXX_COMPILER_ID:MSVC>)
set(warn_compile_options
    $<${is_gcc}:-Wall -Wextra -pedantic-errors>
    $<${is_clang}:-Wall -Wextra -pedantic-errors>
    $<${is_msvc}:/W4 /wd4455>
)
set(werror_compile_option
    $<$<OR:${is_gcc},${is_clang}>:-Werror>
    $<${is_msvc}:/WX>
)
set(compile_options
    $<$<BOOL:${REMOTEMO_WARNINGS}>:${warn_compile_options}>
    $<$<BOOL:${REMOTEMO_WERROR}>:${werror_compile_option}>
)

# Find SDL2
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/sdl2")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# The remotemo-library itself:
add_library(remotemo
    src/remotemo.cpp
    src/exceptions.cpp
    src/keyboard.cpp
    src/config.cpp
    src/engine.cpp
    src/window.cpp
    src/renderer.cpp
    src/texture.cpp
    src/font.cpp
    src/background.cpp
    src/text_display.cpp
)
target_include_directories(remotemo PRIVATE
    include/
    "${PROJECT_BINARY_DIR}/src"
)
  # (How to prevent warnings in header files when used in other projects:
  # https://www.foonathan.net/2018/10/cmake-warnings )
target_include_directories(remotemo SYSTEM INTERFACE
    include/
)

target_compile_options(remotemo PRIVATE
    ${compile_options}
)
target_compile_definitions(remotemo PRIVATE $<$<CONFIG:Debug>:DEBUG>)

if (TARGET SDL2::Core)
    target_link_libraries(remotemo PRIVATE
        SDL2::Core
        SDL2::Image
    )
else()
    target_link_libraries(remotemo PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
    )
endif()

# Build samples
IF (REMOTEMO_BUILD_SAMPLES)
    add_executable(hello_sample samples/hello_sample.cpp)
    # It is enough for all the samples that the following runs for a single
    # one of them:
    add_custom_command(
        TARGET hello_sample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/res
                $<TARGET_FILE_DIR:hello_sample>/res
        COMMENT "Copying resources into samples build directory"
    )
    if (TARGET SDL2::Main)
        target_link_libraries(hello_sample PRIVATE remotemo SDL2::Main)
    else()
        target_link_libraries(hello_sample PRIVATE remotemo SDL2::SDL2main)
    endif()
    target_compile_options(hello_sample PRIVATE ${compile_options})
    target_compile_definitions(hello_sample PRIVATE $<$<CONFIG:Debug>:DEBUG>)

    add_executable(wrap_n_scroll_test samples/wrap_n_scroll_test.cpp)
    if (TARGET SDL2::Main)
        target_link_libraries(wrap_n_scroll_test PRIVATE remotemo SDL2::Main)
    else()
        target_link_libraries(wrap_n_scroll_test PRIVATE
            remotemo
            SDL2::SDL2main)
    endif()
    target_compile_options(wrap_n_scroll_test PRIVATE ${compile_options})
    target_compile_definitions(wrap_n_scroll_test PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
    )
endif()

# Build tests
if (REMOTEMO_BUILD_TESTS)
    add_subdirectory(tests)
endif()
