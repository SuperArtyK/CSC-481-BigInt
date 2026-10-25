
# Verify that all subproject executables compile and link.
# These checks never execute the resulting programs.

foreach(target IN ITEMS
    BigInt
    BigInt_Math
    EvaluatorDemo
)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR
            "Build check target does not exist: ${target}"
        )
    endif()

    add_test(
        NAME "build.${target}"
        COMMAND "${CMAKE_COMMAND}"
            --build "${CMAKE_BINARY_DIR}"
            --config "$<CONFIG>"
            --target "${target}"
    )

    set_tests_properties("build.${target}" PROPERTIES
        LABELS "build"
        RUN_SERIAL TRUE
    )
endforeach()
