// edge.hpp
#pragma once
#include <common.hpp>
#include <memory>

namespace HalfMesh {

    class Edge {
    public:
        Edge(std::shared_ptr<Vertex> a,
             std::shared_ptr<Vertex> b)
          : v1_(a),
            v2_(b),
            handle_(std::numeric_limits<unsigned int>::max()),
            boundary_(false)
        {}

        ~Edge() = default;

        //— Accessors ——
        std::shared_ptr<Vertex>    get_vertex_one()   const { return v1_.lock(); }
        std::shared_ptr<Vertex>    get_vertex_two()   const { return v2_.lock(); }
        unsigned int               handle()           const { return handle_; }
        bool                       is_boundary()      const { return boundary_; }
        std::shared_ptr<HalfEdge>  get_one_half_edge()const { return one_half_edge_.lock(); }

        //— Mutators ——
        void set_handle(unsigned int h)                 { handle_ = h; }
        void set_boundary(bool b)                       { boundary_ = b; }
        void set_one_half_edge(std::shared_ptr<HalfEdge> he) { one_half_edge_ = he; }

    private:
        std::weak_ptr<Vertex>   v1_, v2_;
        unsigned int            handle_;
        std::weak_ptr<HalfEdge> one_half_edge_;
        bool                    boundary_;
    };

} // namespace HalfMesh