#include "triMesh.hpp"
#include <unordered_set>
#include <queue>
#include <vector>

namespace halfMesh {
    bool triMesh::is_multiply_connected() const {
        return num_connected_components() > 1;
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

    bool triMesh::has_boundary() const {
        for (auto &e: edges_) {
            if (e->is_boundary()) {
                return true;
            }
        }
        return false;
    }

    int triMesh::euler_characteristic() const {
        return static_cast<int>(vertices_.size()  - edges_.size() + faces_.size());
    }

    int triMesh::genus() const {
        const int chi = euler_characteristic();
        const int b = has_boundary() ? compute_number_of_holes() : 0;
        return (2 - b - chi) / 2;
    }

    bool triMesh::is_edge_manifold() const {
        for (auto &e: edges_) {
            const auto he0 = e->get_one_half_edge();
            if (!he0) continue;
            int count = 1;
            if (he0->get_opposing_half_edge()) ++count;
            if (count > 2) return false;
        }
        return true;
    }

    bool triMesh::is_manifold() const {
        if (!is_edge_manifold()) return false;

        for (auto &v: vertices_) {
            auto inc_faces = one_ring_faces_of_a_vertex(v);
            if (inc_faces.empty()) continue;

            std::unordered_set<unsigned> seen;
            std::queue<facePtr> Q;
            auto it = inc_faces.begin();
            Q.push(*it);
            seen.insert((*it)->get_handle());

            while (!Q.empty()) {
                auto f = Q.front();
                Q.pop();
                for (auto &nbr: adjacent_faces(f)) {
                    if (inc_faces.count(nbr) == 0) continue;
                    unsigned h = nbr->get_handle();
                    if (seen.insert(h).second)
                        Q.push(nbr);
                }
            }

            if (seen.size() != inc_faces.size())
                return false;
        }

        return true;
    }

    bool triMesh::is_oriented() const {
        for (auto &he: half_edges_) {
            const auto opp = he->get_opposing_half_edge();
            if (!opp) continue;
            const auto a1 = he->get_vertex_one();
            const auto b1 = he->get_vertex_two();
            const auto a2 = opp->get_vertex_one();
            const auto b2 = opp->get_vertex_two();
            if (!(a1 && b1 && a2 && b2)) return false;
            if (a1->get_handle() != b2->get_handle() ||
                b1->get_handle() != a2->get_handle())
                return false;
        }
        return true;
    }

    bool triMesh::is_triangular() const {
        return true;
    }

    double triMesh::surface_area() const {
        double total = 0.0;

        for (const auto &f : faces_) {
            // unpack the three corner vertices
            auto [v0, v1, v2] = f->get_vertices();

            // pull out their 3D positions
            const Eigen::Vector3d p0 = v0->get_position();
            const Eigen::Vector3d p1 = v1->get_position();
            const Eigen::Vector3d p2 = v2->get_position();

            // standard triangle area
            total += 0.5 * (p1 - p0).cross(p2 - p0).norm();
        }

        return total;
    }

    size_t triMesh::num_connected_components() const {
        const size_t N = vertices_.size();
        std::vector<char> visited(N, 0);
        size_t comps = 0;

        for (auto &v0: vertices_) {
            const unsigned h0 = v0->get_handle();
            if (visited[h0]) continue;
            ++comps;
            std::queue<vertexPtr> Q;
            Q.push(v0);
            visited[h0] = 1;

            while (!Q.empty()) {
                const auto v = Q.front();
                Q.pop();
                for (const auto &he: v->get_outgoing_half_edges()) {
                    if (auto w = he->get_vertex_two(); w && !visited[w->get_handle()]) {
                        visited[w->get_handle()] = 1;
                        Q.push(w);
                    }
                }
                for (const auto &he: v->get_incoming_half_edges()) {
                    if (auto w = he->get_vertex_one(); w && !visited[w->get_handle()]) {
                        visited[w->get_handle()] = 1;
                        Q.push(w);
                    }
                }
            }
        }

        return comps;
    }
} // namespace HalfMesh
