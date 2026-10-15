cmake_minimum_required(VERSION 3.23)
project(CSC_481_BigInt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest and GoogleTest are optional at configure time.
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        # GoogleTest v1.18.0, pinned to its immutable release commit.
        GIT_TAG 063de7e9578f82b369302001269680b4b1553359
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    include(GoogleTest)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ModuleTests.cmake")
endif()

# Modules are registered in dependency order.
add_subdirectory("BigInt")
add_subdirectory("BigInt_Math")
add_subdirectory("demo")

# Add any project-specific CMake customization below.


# Register compilation checks after all targets exist.
if(BUILD_TESTING)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildChecks.cmake")
endif()
