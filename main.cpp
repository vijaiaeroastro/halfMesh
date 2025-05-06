// main.cpp

#include "triMesh.hpp"
#include <iostream>
#include <cassert>

using namespace halfMesh;

// Hand-built test mesh
triMesh create_mesh() {
    triMesh mesh;
    const auto v1 = mesh.add_vertex(0.0, 0.0, 0.0);
    const auto v2 = mesh.add_vertex(1.0, 0.0, 0.0);
    const auto v3 = mesh.add_vertex(0.0, 0.5, 0.0);
    const auto v4 = mesh.add_vertex(1.5, 0.5, 0.0);
    const auto v5 = mesh.add_vertex(2.5, 0.0, 0.0);

    mesh.add_face(v1, v2, v3);
    mesh.add_face(v2, v4, v3);
    mesh.add_face(v2, v5, v4);

    mesh.complete_mesh();
    return mesh;
}

triMesh simpleMeshWithDuplicateVertices() {
    triMesh mesh;

    const auto v1 = mesh.add_vertex(0.0, 0.5, 0.0);
    const auto v2 = mesh.add_vertex(0.5, 1.0, 0.0);
    const auto v3 = mesh.add_vertex(0.5, 1.0, 0.0);
    const auto v4 = mesh.add_vertex(1.0, 0.5, 0.0);
    const auto v5 = mesh.add_vertex(0.5, 0.0, 0.0);
    const auto v6 = mesh.add_vertex(0.5, 0.0, 0.0);

    // 1,2,6
    mesh.add_face(v1, v2, v6);
    // 5,3,4
    mesh.add_face(v5, v3, v4);

    mesh.complete_mesh();

    mesh.save("interview_dups.stl");

    return mesh;
}

void stlReadWriteTest() {
    triMesh mesh;
    mesh.read("/home/vijai/schutzDrive/Documents/GEOMETRIES/nut_sample.stl");
    mesh.save("interview_mesh_new.stl");
}

void meshSplitTest() {
    const auto duplicateFreeMesh = simpleMeshWithDuplicateVertices();
    std::cout << "Num Components : " << duplicateFreeMesh.num_connected_components() << std::endl;
}

int main() {
    triMesh mesh;
    mesh.read("../data/Sphere.stl");
    mesh.delete_face(mesh.get_face(0));
    std::cout << "Sphere surface area : " << mesh.surface_area() << std::endl;
    auto bbox = mesh.axis_aligned_bounding_box();
    std::cout << "Bbox : " << bbox.min() << " / " << bbox.max() << std::endl;
    // mesh.delete_vertex(mesh.get_vertex(5));
    // mesh.delete_vertex(mesh.get_vertex(10));
    mesh.save("sphere_out.stl");
    return 0;
}

int main_old() {
    meshSplitTest();

    const auto mesh = create_mesh();

    std::cout << mesh << std::endl;

    // --- Basic counts ---
    assert(mesh.get_vertices().size()    == 5);
    assert(mesh.get_faces().size()       == 3);
    assert(mesh.get_edges().size()       == 7);
    assert(mesh.get_half_edges().size()  == 9);

    // --- Connectivity tests ---
    assert(!mesh.is_multiply_connected());
    assert(mesh.compute_number_of_holes() == 1);

    // --- One-ring tests ---
    // Vertex 0 should have exactly 1 neighbor
    const auto v0 = mesh.get_vertices()[0];
    assert(mesh.one_ring_vertex_of_a_vertex(v0).size() == 1);
    // Vertex 1 should have exactly 3 neighbors
    const auto v1 = mesh.get_vertices()[1];
    assert(mesh.one_ring_vertex_of_a_vertex(v1).size() == 3);

    // --- Face adjacency ---
    // Face 0 should be adjacent only to Face 1
    const auto f0  = mesh.get_faces()[0];
    const auto adj = mesh.adjacent_faces(f0);
    assert(adj.size() == 1);

    // --- Incident faces around vertex 1 ---
    // Vertex 1 touches 3 faces
    const auto vf = mesh.one_ring_faces_of_a_vertex(v1);
    assert(vf.size() == 3);

    std::cout << "All tests passed!\n\n";

    // --- Optional diagnostics ---
    std::cout << "Multiply Connected? "
              << (mesh.is_multiply_connected() ? "Yes" : "No")
              << "\n\n";
    std::cout << "N Components : " << mesh.num_connected_components() << std::endl;
    std::cout << "Genus        : " << mesh.genus() << std::endl;
    std::cout << "Is Manifold  : " << mesh.is_manifold() << std::endl;

    // Loop each face’s half-edge cycle
    for (auto& f : mesh.get_faces()) {
        auto start = f->get_one_half_edge();
        if (!start) continue;

        std::cout << f << " : ";
        auto he = start;
        do {
            std::cout << he << " ";
            he = mesh.get_next_half_edge(he, f);
            if (!he) break;
        } while (he != start);
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Loop each vertex’s incoming/outgoing half-edges
    for (auto& v : mesh.get_vertices()) {
        auto inc = v->get_incoming_half_edges();
        auto out = v->get_outgoing_half_edges();
        std::cout << v
                  << " inc:" << inc.size()
                  << " out:" << out.size() << std::endl;
    }
    std::cout << std::endl;

    // Count boundary edges
    {
        size_t count = 0;
        for (auto& e : mesh.get_edges())
            if (e->is_boundary()) ++count;
        std::cout << "Boundary edges: " << count << std::endl;
    }

    // Count boundary half-edges
    {
        size_t count = 0;
        for (auto& he : mesh.get_half_edges())
            if (he->is_boundary()) ++count;
        std::cout << "Boundary half-edges: " << count << std::endl;
    }

    return 0;
}