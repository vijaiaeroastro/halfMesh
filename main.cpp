#include "mesh.hpp"
#include <iostream>

using namespace HalfMesh;

// Hand-built test mesh
Mesh create_mesh() {
    Mesh mesh;
    auto v1 = mesh.add_vertex(0.0, 0.0, 0.0);
    auto v2 = mesh.add_vertex(1.0, 0.0, 0.0);
    auto v3 = mesh.add_vertex(0.0, 0.5, 0.0);
    auto v4 = mesh.add_vertex(1.5, 0.5, 0.0);
    auto v5 = mesh.add_vertex(2.5, 0.0, 0.0);

    mesh.add_face(v1, v2, v3);
    mesh.add_face(v2, v4, v3);
    mesh.add_face(v2, v5, v4);

    mesh.complete_mesh();
    return mesh;
}

// Loop each face’s half-edge cycle
void loop_through_half_edges_inside_a_face(const Mesh& mesh) {
    for (auto& f : mesh.get_faces()) {
        auto start = f->get_one_half_edge();
        if (!start) continue;

        std::cout << "Face " << f->handle() << ": ";
        auto he = start;
        do {
            std::cout << he->handle() << " ";
            he = mesh.get_next_half_edge(he, f);
            if (!he) break;
        } while (he != start);
        std::cout << "\n";
    }
}

// Loop each vertex’s incoming/outgoing half-edges
void loop_through_vertices_in_a_mesh(const Mesh& mesh) {
    for (auto& v : mesh.get_vertices()) {
        auto inc = v->incoming_half_edges();
        auto out = v->outgoing_half_edges();
        std::cout << "Vertex " << v->handle()
                  << " inc:" << inc.size()
                  << " out:" << out.size() << "\n";
    }
}

// Count boundary edges
void detect_boundary_edges(const Mesh& mesh) {
    size_t count = 0;
    for (auto& e : mesh.get_edges())
        if (e->is_boundary()) ++count;
    std::cout << "Boundary edges: " << count << "\n";
}

// Count boundary half-edges
void detect_boundary_half_edges(const Mesh& mesh) {
    size_t count = 0;
    for (auto& he : mesh.get_half_edges())
        if (he->is_boundary()) ++count;
    std::cout << "Boundary half-edges: " << count << "\n";
}

int main() {
    // Build our simple mesh
    Mesh mesh = create_mesh();
    std::cout << "Mesh vertices : " << mesh.get_vertices().size() << "\n";
    std::cout << "Mesh faces    : " << mesh.get_faces().size() << "\n";

    // Diagnostics
    std::cout << "Multiply Connected? "
              << (mesh.is_multiply_connected() ? "Yes" : "No")
              << "\n\n";

    // loop_through_half_edges_inside_a_face(mesh);
    // std::cout << "\n";

    // loop_through_vertices_in_a_mesh(mesh);
    // std::cout << "\n";

    detect_boundary_edges(mesh);
    detect_boundary_half_edges(mesh);

    return 0;
}