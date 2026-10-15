# Generated once. You may edit this CMakeLists.txt freely.
# src/ is inherited (except from the main project); private/ is executable-only.
# tests/ files build into a separate GoogleTest executable, never the playground.

add_library(BigInt_sources INTERFACE)
target_include_directories(BigInt_sources INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

file(GLOB_RECURSE _module_public CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)
target_sources(BigInt_sources INTERFACE ${_module_public})

file(GLOB_RECURSE _module_private CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cxx"
)
add_executable(BigInt main.cpp ${_module_private})
target_link_libraries(BigInt PRIVATE BigInt_sources)

if(BUILD_TESTING)
    add_module_tests(BigInt BigInt_sources)
endif()
