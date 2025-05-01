#include <iostream>

#include "triMesh.hpp"
#include <utility>      // for std::swap

namespace halfMesh {
    // Canonicalization helpers (could also live in a detail header)
    inline EdgeKey make_edge_key(unsigned i, unsigned j) {
        if (i > j) std::swap(i, j);
        return {i, j};
    }

    inline FaceKey make_face_key(unsigned a, unsigned b, unsigned c) {
        std::array<unsigned, 3> v{a, b, c};
        std::sort(v.begin(), v.end());
        return {v[0], v[1], v[2]};
    }

    triMesh::triMesh() = default;

    triMesh::~triMesh() = default;

    void triMesh::clear_data() {
        vertices_.clear();
        half_edges_.clear();
        edges_.clear();
        faces_.clear();
        handle_to_vertex_.clear();
        handle_to_half_edge_.clear();
        handle_to_edge_.clear();
        handle_to_face_.clear();
        edge_lookup_.clear();
        face_lookup_.clear();
        half_edge_lookup_.clear();
        vertex_data_store.clear();
        edge_data_store.clear();
        face_data_store.clear();
        next_vertex_handle_ = 0;
        next_half_edge_handle_ = 0;
        next_edge_handle_ = 0;
        next_face_handle_ = 0;
    }

    // Core mutators
    triMesh::VertexPtr triMesh::add_vertex(double x, double y, double z) {
        auto v = std::make_shared<vertex>(x, y, z);
        unsigned h = next_vertex_handle_++;
        v->set_handle(h);
        vertices_.push_back(v);
        handle_to_vertex_[h] = v;
        // std::cout << "Added vertex : " << h << " with coordinates : " << x << "," << y << "," << z << std::endl;
        return v;
    }

    triMesh::HalfEdgePtr triMesh::add_half_edge(const VertexPtr &v1,
                                          const VertexPtr &v2,
                                          const FacePtr &f) {
        const HalfEdgeKey key{v1->handle(), v2->handle()};
        if (const auto it = half_edge_lookup_.find(key); it != half_edge_lookup_.end())
            return it->second;

        auto he = std::make_shared<halfedge>(v1, v2);
        const unsigned h = next_half_edge_handle_++;
        he->set_handle(h);
        he->set_parent_face(f->handle());

        // link opposites
        const auto rev = std::make_pair(v2->handle(), v1->handle());
        if (const auto rit = half_edge_lookup_.find(rev); rit != half_edge_lookup_.end()) {
            const auto opp = rit->second->handle();
            he->set_opposing_half_edge(opp);
            rit->second->set_opposing_half_edge(h);
        }

        v1->add_outgoing_half_edge(he);
        v2->add_incoming_half_edge(he);

        half_edges_.push_back(he);
        handle_to_half_edge_[h] = he;
        half_edge_lookup_[key] = he;
        return he;
    }

    triMesh::EdgePtr triMesh::add_edge(const VertexPtr &v1,
                                 const VertexPtr &v2,
                                 const FacePtr &f) {
        const auto key = make_edge_key(v1->handle(), v2->handle());
        if (const auto it = edge_lookup_.find(key); it != edge_lookup_.end()) {
            const auto e = handle_to_edge_[it->second];
            const auto he = add_half_edge(v1, v2, f);
            he->set_parent_edge(e->handle());
            e->set_one_half_edge(he);
            // std::cout << "----> Found an existing edge : "
            // << e->handle() << " between " << e->get_vertex_one()->handle() << "," << e->get_vertex_two()->handle() << std::endl;
            return e;
        }

        auto e = std::make_shared<edge>(v1, v2);
        const unsigned h = next_edge_handle_++;
        e->set_handle(h);
        edges_.push_back(e);
        handle_to_edge_[h] = e;
        edge_lookup_[key] = h;

        const auto he = add_half_edge(v1, v2, f);
        he->set_parent_edge(h);
        e->set_one_half_edge(he);

        // std::cout << "----> Created a new edge : "
        // << e->handle() << " between " << e->get_vertex_one()->handle() << "," << e->get_vertex_two()->handle() << std::endl;
        return e;
    }

    triMesh::FacePtr triMesh::add_face(const VertexPtr &v1,
                                 const VertexPtr &v2,
                                 const VertexPtr &v3) {
        const auto key = make_face_key(v1->handle(), v2->handle(), v3->handle());
        if (const auto it = face_lookup_.find(key); it != face_lookup_.end())
            return handle_to_face_[it->second];

        // 1) create the Face
        auto f = std::make_shared<face>(v1, v2, v3);
        const unsigned fh = next_face_handle_++;
        f->set_handle(fh);
        faces_.push_back(f);
        handle_to_face_[fh] = f;
        face_lookup_[key] = fh;

        // 2) add the other two edges
        const auto fE1 = add_edge(v1, v2, f);
        const auto fE2 = add_edge(v2, v3, f);
        const auto fE3 = add_edge(v3, v1, f);

        // 3) store one of the half edges on the face
        f->set_one_half_edge(fE1->get_one_half_edge());

        return f;
    }

    void triMesh::complete_mesh() {
        if (faces_.empty()) return;

        // mark half-edge boundaries
        for (auto& he : half_edges_) {
            // std::cout << "HE : " << he->handle() << "->" << he->get_opposing_half_edge() << std::endl;
            he->set_boundary(
                he->get_opposing_half_edge() == std::numeric_limits<unsigned>::max()
            );
        }

        // mark edge boundaries
        for (auto& e : edges_) {
            e->set_boundary(
                e->get_one_half_edge()->is_boundary()
            );
        }
    }


    // trivial handle‐->object
    triMesh::VertexPtr triMesh::get_vertex(unsigned h) const { return handle_to_vertex_.at(h); }
    triMesh::HalfEdgePtr triMesh::get_half_edge(unsigned h) const { return handle_to_half_edge_.at(h); }
    triMesh::EdgePtr triMesh::get_edge(unsigned h) const { return handle_to_edge_.at(h); }
    triMesh::FacePtr triMesh::get_face(unsigned h) const { return handle_to_face_.at(h); }
} // namespace HalfMesh
