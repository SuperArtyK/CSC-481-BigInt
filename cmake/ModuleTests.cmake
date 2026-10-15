# Shared GoogleTest registration for one-shot generated projects.
# The function is called from each module's CMakeLists.txt.
# A module's tests/ tree is optional: no sources means no test executable.
function(add_module_tests module_name sources_target)
    if(NOT BUILD_TESTING)
        return()
    endif()

    file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cxx"
    )
    if(NOT test_sources)
        return()
    endif()

    # Defer fetching GoogleTest until at least one test source exists.
    # This lets newly generated empty projects configure without network access.
    if(NOT TARGET GTest::gtest_main)
        FetchContent_MakeAvailable(googletest)
    endif()

    set(test_target "${module_name}_tests")
    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target} PRIVATE
        ${sources_target}
        GTest::gtest_main
    )
    gtest_discover_tests(${test_target}
        TEST_PREFIX "${module_name}."
        PROPERTIES
            LABELS "${module_name}"
            TIMEOUT 60
    )
endfunction()
