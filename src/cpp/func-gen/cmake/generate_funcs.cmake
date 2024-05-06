function(generate_funcs HDR SRC VER)
    # Get the directory for the header file and make sure it exists.
    get_filename_component(HDR_DIR "${HDR}" PATH)
    file(MAKE_DIRECTORY "${HDR_DIR}")

    # Get the directory for the source file and make sure it exists.
    get_filename_component(SRC_DIR "${SRC}" PATH)
    file(MAKE_DIRECTORY "${SRC_DIR}")

    # Add a command to do the generation. Note the $<TARGET_FILE:func-gen>
    # dependency; this makes cmake correctly recompile the generator and
    # regenerate the files with it if its source files change.
    add_custom_command(
        COMMAND func-gen "${HDR}" "${SRC}" "${VER}"
        OUTPUT "${HDR}" "${SRC}"
        DEPENDS func-gen
    )
endfunction()
