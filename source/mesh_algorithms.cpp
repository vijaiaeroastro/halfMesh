#include "triMesh.hpp"
#include <unordered_set>
#include <vector>

namespace halfMesh {
    bool triMesh::is_multiply_connected() const {
        if (faces_.empty()) return false;

        std::unordered_set<unsigned> visited;
        std::vector<facePtr> stack;
        stack.push_back(faces_[0]);
        visited.insert(faces_[0]->get_handle());

        while (!stack.empty()) {
            auto f = stack.back();
            stack.pop_back();

            // Start with the face's stored half-edge
            auto he = f->get_one_half_edge();
            // Walk the three edges of this triangle
            for (int i = 0; i < 3 && he; ++i) {
                if (i > 0) he = get_next_half_edge(he, f);

                auto opposingHalfEdge = he->get_opposing_half_edge();
                if (! opposingHalfEdge)
                    continue; // boundary, no neighbor

                if (auto nf = opposingHalfEdge->get_parent_face(); nf && visited.insert(nf->get_handle()).second) {
                    stack.push_back(nf);
                }
            }
        }

        // If we reached every face, it's simply connected
        return visited.size() != faces_.size();
    }

    unsigned triMesh::compute_number_of_holes() const {
        // collect boundary half-edges
        std::vector<halfEdgePtr> boundaries;
        for (auto &he: half_edges_) {
            if (he->is_boundary())
                boundaries.push_back(he);
        }

        std::unordered_set<unsigned> used;
        unsigned loops = 0;
        for (const auto &start: boundaries) {
            unsigned h0 = start->get_handle();
            if (used.count(h0)) continue;
            auto cur = start;
            do {
                used.insert(cur->get_handle());
                // next boundary around vertex_two
                auto v = cur->get_vertex_two();
                halfEdgePtr next = nullptr;
                for (auto &cand: v->get_outgoing_half_edges()) {
                    if (cand != cur && cand->is_boundary()) {
                        next = cand;
                        break;
                    }
                }
                cur = next;
            } while (cur && cur->get_handle() != h0);
            ++loops;
        }
        return loops;
    }
} // namespace HalfMesh
