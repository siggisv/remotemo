# Generate documents with Doxygen

function(remotemo_build_docs_w_doxygen)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs)
        set(DOXYFILE_OUT ${DOXYGEN_OUTPUT_DIRECTORY}/Doxyfile)
        configure_file(
            "${PROJECT_SOURCE_DIR}/Doxyfile.in"
            "${DOXYFILE_OUT}"
            @ONLY
        )
        add_custom_target(docs
            COMMAND Doxygen::doxygen ${DOXYFILE_OUT}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/include
            COMMENT "Generating docs"
        )
    else(DOXYGEN_FOUND)
        message("Doxygen needs to be installed to generate the documentation")
    endif(DOXYGEN_FOUND)
endfunction()

remotemo_build_docs_w_doxygen()
