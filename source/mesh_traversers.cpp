#include "triMesh.hpp"
#include <vector>

namespace halfMesh {
    triMesh::HalfEdgePtr triMesh::get_next_half_edge(const HalfEdgePtr &he,
                                                     const FacePtr &f) const {
        auto v2 = he->get_vertex_two();
        for (auto &cand: v2->outgoing_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    triMesh::HalfEdgePtr triMesh::get_previous_half_edge(const HalfEdgePtr &he,
                                                         const FacePtr &f) const {
        auto v1 = he->get_vertex_one();
        for (auto &cand: v1->incoming_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    triMesh::FacePtr triMesh::get_one_neighbour_face(const FacePtr &f) const {
        auto he = f->get_one_half_edge();
        if (!he) return nullptr;
        unsigned opp = he->get_opposing_half_edge();
        if (opp == std::numeric_limits<unsigned>::max()) return nullptr;
        return get_face(get_half_edge(opp)->get_parent_face());
    }
} // namespace HalfMesh
