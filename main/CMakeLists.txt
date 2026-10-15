# Generated once. You may edit this CMakeLists.txt freely.
# Source files placed under src/ and private/ are discovered recursively.

file(GLOB_RECURSE _module_private CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cxx"
)
file(GLOB_RECURSE _module_public CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)
add_executable(main main.cpp ${_module_public} ${_module_private})
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(main PRIVATE BigInt_sources BigInt_Math_sources)
