#include "mesh.hpp"
#include <unordered_set>
#include <vector>

namespace HalfMesh {
    Mesh::HalfEdgePtr Mesh::get_next_half_edge(const HalfEdgePtr &he,
                                               const FacePtr &f) const {
        auto v2 = he->get_vertex_two();
        for (auto &cand: v2->outgoing_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    Mesh::HalfEdgePtr Mesh::get_previous_half_edge(const HalfEdgePtr &he,
                                                   const FacePtr &f) const {
        auto v1 = he->get_vertex_one();
        for (auto &cand: v1->incoming_half_edges()) {
            if (cand != he && cand->get_parent_face() == f->handle())
                return cand;
        }
        return nullptr;
    }

    Mesh::FacePtr Mesh::get_one_neighbour_face(const FacePtr &f) const {
        auto he = f->get_one_half_edge();
        if (!he) return nullptr;
        unsigned opp = he->get_opposing_half_edge();
        if (opp == std::numeric_limits<unsigned>::max()) return nullptr;
        return get_face(get_half_edge(opp)->get_parent_face());
    }

    bool Mesh::is_multiply_connected() const {
        if (faces_.empty()) return false;

        std::unordered_set<unsigned> visited;
        std::vector<FacePtr> stack;
        stack.push_back(faces_[0]);
        visited.insert(faces_[0]->handle());

        while (!stack.empty()) {
            auto f = stack.back();
            stack.pop_back();

            // Start with the face's stored half-edge
            auto he = f->get_one_half_edge();
            // Walk the three edges of this triangle
            for (int i = 0; i < 3 && he; ++i) {
                if (i > 0) he = get_next_half_edge(he, f);

                unsigned opph = he->get_opposing_half_edge();
                if (opph == std::numeric_limits<unsigned>::max())
                    continue; // boundary, no neighbor

                auto he_opp = get_half_edge(opph);
                auto nf = get_face(he_opp->get_parent_face());
                if (nf && visited.insert(nf->handle()).second) {
                    stack.push_back(nf);
                }
            }
        }

        // If we reached every face, it's simply connected
        return visited.size() != faces_.size();
    }

    unsigned Mesh::compute_number_of_holes() const {
        // collect boundary half-edges
        std::vector<HalfEdgePtr> boundaries;
        for (auto &he: half_edges_)
            if (he->is_boundary())
                boundaries.push_back(he);

        std::unordered_set<unsigned> used;
        unsigned loops = 0;
        for (auto &start: boundaries) {
            unsigned h0 = start->handle();
            if (used.count(h0)) continue;
            auto cur = start;
            do {
                used.insert(cur->handle());
                // next boundary around vertex_two
                auto v = cur->get_vertex_two();
                HalfEdgePtr next = nullptr;
                for (auto &cand: v->outgoing_half_edges()) {
                    if (cand != cur && cand->is_boundary()) {
                        next = cand;
                        break;
                    }
                }
                cur = next;
            } while (cur && cur->handle() != h0);
            ++loops;
        }
        return loops;
    }
} // namespace HalfMesh
