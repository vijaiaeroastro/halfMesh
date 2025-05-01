#include "triMesh.hpp"
#include <vector>
#include <unordered_set>

namespace halfMesh {
    triMesh::halfEdgePtr triMesh::get_next_half_edge(const halfEdgePtr &he,
                                                     const facePtr &f) const {
        auto v2 = he->get_vertex_two();
        for (auto &cand: v2->outgoing_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    triMesh::halfEdgePtr triMesh::get_previous_half_edge(const halfEdgePtr &he,
                                                         const facePtr &f) const {
        auto v1 = he->get_vertex_one();
        for (auto &cand: v1->incoming_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    triMesh::facePtr triMesh::get_one_neighbour_face(const facePtr &f) const {
        auto he = f->get_one_half_edge();
        if (!he) return nullptr;
        unsigned opp = he->get_opposing_half_edge();
        if (opp == std::numeric_limits<unsigned>::max()) return nullptr;
        return get_face(get_half_edge(opp)->get_parent_face());
    }

    std::vector<triMesh::vertexPtr> triMesh::vertex_one_ring(const vertexPtr &v) const {
        std::vector<vertexPtr> out;
        // for each outgoing half‐edge v→u, collect u
        for (auto &he: v->outgoing_half_edges()) {
            auto nbr = he->get_vertex_two();
            out.push_back(nbr);
        }
        return out;
    }

    std::vector<triMesh::facePtr> triMesh::vertex_face_ring(const vertexPtr &v) const {
        std::vector<facePtr> out;
        // any half‐edge touching v gives you its parent face
        std::unordered_set<unsigned> seen;
        for (auto &he: v->outgoing_half_edges()) {
            unsigned fh = he->get_parent_face();
            if (seen.insert(fh).second)
                out.push_back(get_face(fh));
        }
        for (auto &he: v->incoming_half_edges()) {
            unsigned fh = he->get_parent_face();
            if (seen.insert(fh).second)
                out.push_back(get_face(fh));
        }
        return out;
    }

    std::vector<triMesh::facePtr> triMesh::face_adjacent(const facePtr &f) const {
        std::vector<facePtr> out;
        // start with the face's three half‐edges
        auto he0 = f->get_one_half_edge();
        halfEdgePtr he = he0;
        for (int i = 0; i < 3 && he; ++i) {
            if (i > 0) he = get_next_half_edge(he, f);
            // cross to the opposite
            unsigned opph = he->get_opposing_half_edge();
            if (opph == std::numeric_limits<unsigned>::max())
                continue;
            auto he2 = get_half_edge(opph);
            auto nf = get_face(he2->get_parent_face());
            // skip if it's the same face
            if (nf && nf->handle() != f->handle())
                out.push_back(nf);
        }
        return out;
    }
} // namespace HalfMesh
