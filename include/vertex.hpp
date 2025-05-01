#pragma once

#include <common.hpp>
#include <half_edge.hpp>
#include <memory>
#include <vector>

namespace HalfMesh {

    class Vertex {
    public:
        Vertex(double x, double y, double z)
          : x_(x), y_(y), z_(z),
            handle_(std::numeric_limits<unsigned int>::max())
        {}

        ~Vertex() = default;

        //— Accessors ——
        unsigned int handle() const           { return handle_; }
        double       get_x()  const           { return x_; }
        double       get_y()  const           { return y_; }
        double       get_z()  const           { return z_; }

        std::vector<std::shared_ptr<HalfEdge>> incoming_half_edges() const {
            std::vector<std::shared_ptr<HalfEdge>> out;
            out.reserve(incoming_.size());
            for (auto& wp : incoming_) {
                if (auto sp = wp.lock()) out.push_back(sp);
            }
            return out;
        }

        std::vector<std::shared_ptr<HalfEdge>> outgoing_half_edges() const {
            std::vector<std::shared_ptr<HalfEdge>> out;
            out.reserve(outgoing_.size());
            for (auto& wp : outgoing_) {
                if (auto sp = wp.lock()) out.push_back(sp);
            }
            return out;
        }

        //— Mutators ——
        void set_handle(unsigned int h)                   { handle_ = h; }
        void set_x(double x)                              { x_ = x; }
        void set_y(double y)                              { y_ = y; }
        void set_z(double z)                              { z_ = z; }

        void add_incoming_half_edge(const std::shared_ptr<HalfEdge>& he) {
            incoming_.push_back(he);
        }

        void add_outgoing_half_edge(const std::shared_ptr<HalfEdge>& he) {
            outgoing_.push_back(he);
        }

    private:
        double  x_, y_, z_;
        unsigned int                         handle_;
        std::vector<std::weak_ptr<HalfEdge>> incoming_;
        std::vector<std::weak_ptr<HalfEdge>> outgoing_;
    };

} // namespace HalfMesh