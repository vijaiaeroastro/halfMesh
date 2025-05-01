cmake_minimum_required(VERSION 3.15)
project(halfMesh)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(HDRS
        include/common.hpp
        include/edge.hpp
        include/face.hpp
        include/connectivity.hpp
        include/half_edge.hpp
        include/triMesh.hpp
        include/vertex.hpp
        include/json.hpp)

set(SRCS
        source/mesh_core.cpp
        source/mesh_algorithms.cpp
        source/mesh_traversers.cpp
        source/mesh_geometry.cpp
        source/mesh_io.cpp)

add_library(halfMesh ${HDRS} ${SRCS})

add_executable(halfMeshTest main.cpp)
target_link_libraries(halfMeshTest halfMesh)