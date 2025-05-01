cmake_minimum_required(VERSION 3.15)
project(halfMesh VERSION 1.0 LANGUAGES CXX)

# enforce C++17
set(CMAKE_CXX_STANDARD        17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS      OFF)

# Optionally build tests
option(BUILD_TESTS "Build the halfMeshTest executable" ON)
option(BUILD_SHARED_LIBS "Build Shared library" OFF)

# Source files
set(SRCS
        source/mesh_core.cpp
        source/mesh_algorithms.cpp
        source/mesh_traversers.cpp
        source/mesh_geometry.cpp
        source/mesh_io.cpp
)

# Header files (for IDE integration only)
set(HDRS
        include/common.hpp
        include/connectivity.hpp
        include/vertex.hpp
        include/half_edge.hpp
        include/edge.hpp
        include/face.hpp
        include/triMesh.hpp
        include/json.hpp
)

# Pick shared vs static based on BUILD_SHARED_LIBS
if(BUILD_SHARED_LIBS)
    add_library(halfMesh SHARED
            ${SRCS}
            ${HDRS}
    )
else()
    add_library(halfMesh STATIC
            ${SRCS}
            ${HDRS}
    )
endif()

# Where to find your headers
target_include_directories(halfMesh
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compiler warnings
if(MSVC)
    target_compile_options(halfMesh PRIVATE /W4 /permissive-)
else()
    target_compile_options(halfMesh PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Test executable
if(BUILD_TESTS)
    add_executable(halfMeshTest main.cpp)
    target_link_libraries(halfMeshTest PRIVATE halfMesh)
    target_include_directories(halfMeshTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    enable_testing()
    add_test(NAME halfMeshTest COMMAND halfMeshTest)
endif()