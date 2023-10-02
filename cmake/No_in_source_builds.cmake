# This function will prevent in-source builds
#
function(remotemo_no_in_source_builds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if("${srcdir}" STREQUAL "${bindir}")
    message("=========================================================")
    message("                     Warning!")
    message("             In-source builds are disabled")
    message("         Please use a separate build directory")
    message("")
    message("  Creating a build directory and setting up can be done")
    message("  with a single command from a projects root directory:")
    message("      cmake -S . -B build")
    message("=========================================================")
    message(FATAL_ERROR "Quitting configuration")
  endif()
endfunction()

remotemo_no_in_source_builds()
