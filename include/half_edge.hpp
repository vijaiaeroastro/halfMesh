// half_edge.hpp
#pragma once
#include <common.hpp>
#include <memory>
#include <limits>

namespace HalfMesh {

    class HalfEdge {
    public:
        HalfEdge(std::shared_ptr<Vertex> a,
                 std::shared_ptr<Vertex> b)
          : v1_(a),
            v2_(b),
            handle_(std::numeric_limits<unsigned int>::max()),
            parent_edge_handle_(std::numeric_limits<unsigned int>::max()),
            parent_face_handle_(std::numeric_limits<unsigned int>::max()),
            opposing_half_edge_(std::numeric_limits<unsigned int>::max()),
            boundary_(false)
        {}

        ~HalfEdge() = default;

        //— Accessors ——
        std::shared_ptr<Vertex> get_vertex_one()   const { return v1_.lock(); }
        std::shared_ptr<Vertex> get_vertex_two()   const { return v2_.lock(); }
        unsigned int            handle()          const { return handle_; }
        unsigned int            get_parent_edge() const { return parent_edge_handle_; }
        unsigned int            get_parent_face() const { return parent_face_handle_; }
        unsigned int            get_opposing_half_edge() const { return opposing_half_edge_; }
        bool                    is_boundary()     const { return boundary_; }

        //— Mutators ——
        void set_handle(unsigned int h)                   { handle_ = h; }
        void set_parent_edge(unsigned int h)              { parent_edge_handle_  = h; }
        void set_parent_face(unsigned int h)              { parent_face_handle_  = h; }
        void set_opposing_half_edge(unsigned int h)       { opposing_half_edge_  = h; }
        void set_boundary(bool b)                         { boundary_ = b; }

    private:
        std::weak_ptr<Vertex>   v1_, v2_;
        unsigned int            handle_;
        unsigned int            parent_edge_handle_;
        unsigned int            parent_face_handle_;
        unsigned int            opposing_half_edge_;
        bool                    boundary_;
    };

} // namespace HalfMesh
