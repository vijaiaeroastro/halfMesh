// face.hpp
#pragma once
#include <common.hpp>
#include <memory>
#include <tuple>

namespace HalfMesh {

    class Face {
    public:
        Face(std::shared_ptr<Vertex> a,
             std::shared_ptr<Vertex> b,
             std::shared_ptr<Vertex> c)
          : v1_(a),
            v2_(b),
            v3_(c),
            handle_(std::numeric_limits<unsigned int>::max())
        {}

        ~Face() = default;

        //— Accessors ——
        unsigned int handle() const               { return handle_; }
        bool        is_valid() const              { return handle_ != std::numeric_limits<unsigned int>::max(); }

        std::tuple<std::shared_ptr<Vertex>,
                   std::shared_ptr<Vertex>,
                   std::shared_ptr<Vertex>>
          get_vertices() const
        {
            return { v1_.lock(), v2_.lock(), v3_.lock() };
        }

        std::shared_ptr<HalfEdge> get_one_half_edge() const { return one_half_edge_.lock(); }

        //— Mutators ——
        void set_handle(unsigned int h)                   { handle_ = h; }
        void set_one_half_edge(std::shared_ptr<HalfEdge> he) { one_half_edge_ = he; }

    private:
        std::weak_ptr<Vertex>   v1_, v2_, v3_;
        unsigned int            handle_;
        std::weak_ptr<HalfEdge> one_half_edge_;
    };

} // namespace HalfMesh