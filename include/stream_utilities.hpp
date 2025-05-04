// stream_utilities.hpp
#pragma once

#include <iostream>
#include <memory>

#include "vertex.hpp"
#include "edge.hpp"
#include "half_edge.hpp"
#include "face.hpp"

namespace halfMesh {
    //
    // Raw‐object overloads
    //

    inline std::ostream &operator<<(std::ostream &os, vertex const &v) {
        os << "vertex(" << v.get_handle()
                << ": [" << v.get_x() << ", " << v.get_y() << ", " << v.get_z() << "])";
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, edge const &e) {
        auto a = e.get_vertex_one(), b = e.get_vertex_two();
        os << "edge(" << e.get_handle() << ": "
                << (a ? a->get_handle() : unsigned(-1)) << "->"
                << (b ? b->get_handle() : unsigned(-1))
                << (e.is_boundary() ? " [B]" : "") << ")";
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, halfedge const &he) {
        auto a = he.get_vertex_one(), b = he.get_vertex_two();
        os << "halfedge(" << he.get_handle() << ": "
                << (a ? a->get_handle() : unsigned(-1)) << "->"
                << (b ? b->get_handle() : unsigned(-1));
        if (auto opp = he.get_opposing_half_edge()) {
            os << " opp=" << opp->get_handle();
        } else {
            os << " opp=<none>";
        }
        os << (he.is_boundary() ? " [B]" : "") << ")";
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, face const &f) {
        auto [a, b, c] = f.get_vertices();
        os << "face(" << f.get_handle() << ": "
                << (a ? a->get_handle() : unsigned(-1)) << ","
                << (b ? b->get_handle() : unsigned(-1)) << ","
                << (c ? c->get_handle() : unsigned(-1)) << ")";
        return os;
    }

    //
    // shared_ptr overloads (so you can stream Ptr types directly)
    //

    inline std::ostream &operator<<(std::ostream &os, std::shared_ptr<vertex> const &p) {
        return p ? os << *p : os << "vertex(nullptr)";
    }

    inline std::ostream &operator<<(std::ostream &os, std::shared_ptr<edge> const &p) {
        return p ? os << *p : os << "edge(nullptr)";
    }

    inline std::ostream &operator<<(std::ostream &os, std::shared_ptr<halfedge> const &p) {
        return p ? os << *p : os << "halfedge(nullptr)";
    }

    inline std::ostream &operator<<(std::ostream &os, std::shared_ptr<face> const &p) {
        return p ? os << *p : os << "face(nullptr)";
    }
} // namespace halfMesh
