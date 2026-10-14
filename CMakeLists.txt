cmake_minimum_required(VERSION 3.23)
project(CSC_481_BigInt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modules are registered in dependency order.
add_subdirectory("BigInt")
add_subdirectory("BigInt_Math")
add_subdirectory("main")

# Add any project-specific CMake customization below.
