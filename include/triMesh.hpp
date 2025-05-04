#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <json.hpp>

#include "common.hpp"
#include "connectivity.hpp"
#include "vertex.hpp"
#include "half_edge.hpp"
#include "edge.hpp"
#include "face.hpp"
#include "stream_utilities.hpp"

namespace halfMesh {
    class triMesh {
    public:
        triMesh();

        ~triMesh();

        // Core mutators
        vertexPtr add_vertex(double x, double y, double z);

        halfEdgePtr add_half_edge(const vertexPtr &v1,
                                  const vertexPtr &v2,
                                  const facePtr &f);

        edgePtr add_edge(const vertexPtr &v1,
                         const vertexPtr &v2,
                         const facePtr &f);

        facePtr add_face(const vertexPtr &v1,
                         const vertexPtr &v2,
                         const vertexPtr &v3);

        void complete_mesh();

        // I/O
        void save(const std::string &filename) const;

        void read(const std::string &filename);

        // Traversals & topology
        halfEdgePtr get_next_half_edge(const halfEdgePtr &he, const facePtr &f) const;

        halfEdgePtr get_previous_half_edge(const halfEdgePtr &he, const facePtr &f) const;

        facePtr get_one_neighbour_face(const facePtr &f) const;

        std::unordered_set<facePtr> adjacent_faces(const facePtr &f) const;

        std::unordered_set<facePtr> one_ring_faces_of_a_vertex(const vertexPtr &v) const;

        std::unordered_set<vertexPtr> one_ring_vertex_of_a_vertex(const vertexPtr &v) const;

        // Some algorithms
        bool is_multiply_connected() const;

        unsigned compute_number_of_holes() const;

        bool has_boundary() const;

        int euler_characteristic() const;

        int genus() const;

        bool is_edge_manifold() const;

        bool is_manifold() const;

        bool is_oriented() const;

        bool is_triangular() const;

        size_t num_connected_components() const;

        // Geometry queries
        double get_area(unsigned face_handle) const;

        vertex get_face_normal(unsigned face_handle) const;

        double get_face_angle(unsigned f1, unsigned f2) const;

        // Property API
        template<typename T>
        PropertyStatus add_vertex_property(const std::string &name, T init) {
            if (vertex_data_store.contains(name))
                return PropertyStatus::Exists;
            for (auto &v: vertices_)
                vertex_data_store[name][v->get_handle()] = init;
            return PropertyStatus::Added;
        }

        template<typename T>
        PropertyStatus add_edge_property(const std::string &name, T init) {
            if (edge_data_store.contains(name))
                return PropertyStatus::Exists;
            for (auto &e: edges_)
                edge_data_store[name][e->get_handle()] = init;
            return PropertyStatus::Added;
        }

        template<typename T>
        PropertyStatus add_face_property(const std::string &name, T init) {
            if (face_data_store.contains(name))
                return PropertyStatus::Exists;
            for (auto &f: faces_)
                face_data_store[name][f->get_handle()] = init;
            return PropertyStatus::Added;
        }

        // Inline remove‐property implementation
        PropertyStatus delete_property(const std::string &name, EntityType type) {
            auto &store = (type == EntityType::Vertex
                               ? vertex_data_store
                               : type == EntityType::Edge
                                     ? edge_data_store
                                     : face_data_store);
            if (!store.contains(name))
                return PropertyStatus::DoesNotExist;
            store.erase(name);
            return PropertyStatus::Deleted;
        }

        template<typename T>
        void set_vertex_property(const std::string &name, unsigned h, T val) {
            vertex_data_store[name][h] = val;
        }

        template<typename T>
        void set_edge_property(const std::string &name, unsigned h, T val) {
            edge_data_store[name][h] = val;
        }

        template<typename T>
        void set_face_property(const std::string &name, unsigned h, T val) {
            face_data_store[name][h] = val;
        }

        template<typename T>
        T get_vertex_property(const std::string &name, unsigned h) const {
            return vertex_data_store.at(name).at(h).get<T>();
        }

        template<typename T>
        T get_edge_property(const std::string &name, unsigned h) const {
            return edge_data_store.at(name).at(h).get<T>();
        }

        template<typename T>
        T get_face_property(const std::string &name, unsigned h) const {
            return face_data_store.at(name).at(h).get<T>();
        }

        // Helpers
        vertexPtr get_vertex(unsigned h) const;

        halfEdgePtr get_half_edge(unsigned h) const;

        edgePtr get_edge(unsigned h) const;

        facePtr get_face(unsigned h) const;


        // --- Bulk accessors definitions ---

        const std::vector<vertexPtr> &
        get_vertices() const {
            return vertices_;
        }

        const std::vector<halfEdgePtr> &
        get_half_edges() const {
            return half_edges_;
        }

        const std::vector<edgePtr> &
        get_edges() const {
            return edges_;
        }

        const std::vector<facePtr> &
        get_faces() const {
            return faces_;
        }

    private:
        // I/O routines
        void read_gmsh(const std::string &fn);

        void read_obj(const std::string &fn);

        void read_binary(const std::string &fn);

        void write_gmsh(const std::string &fn) const;

        void write_obj(const std::string &fn) const;

        void write_binary(const std::string &fn) const;

        void write_vtk(const std::string &fn) const;

        // STL I/O
        void write_stl_ascii(const std::string &filename) const;

        void read_stl(const std::string &filename);

        void read_stl_ascii(const std::string &filename);

        void read_stl_binary(const std::string &filename);

        // Cleanup
        void clear_data();

        // Ownership
        std::vector<vertexPtr> vertices_;
        std::vector<halfEdgePtr> half_edges_;
        std::vector<edgePtr> edges_;
        std::vector<facePtr> faces_;

        // Handle → object maps
        std::unordered_map<unsigned, vertexPtr> handle_to_vertex_;
        std::unordered_map<unsigned, halfEdgePtr> handle_to_half_edge_;
        std::unordered_map<unsigned, edgePtr> handle_to_edge_;
        std::unordered_map<unsigned, facePtr> handle_to_face_;

        // Connectivity lookups
        EdgeHandleMap edge_lookup_;
        FaceHandleMap face_lookup_;
        std::unordered_map<HalfEdgeKey,
            halfEdgePtr,
            HalfEdgeKeyHash,
            HalfEdgeKeyEqual> half_edge_lookup_;

        // Per‐entity property stores
        nlohmann::json vertex_data_store;
        nlohmann::json edge_data_store;
        nlohmann::json face_data_store;

        // Next‐free handles
        unsigned next_vertex_handle_ = 0;
        unsigned next_half_edge_handle_ = 0;
        unsigned next_edge_handle_ = 0;
        unsigned next_face_handle_ = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, triMesh const& m) {
        os << "triMesh(V=" << m.get_vertices().size()
           << ", E=" << m.get_edges().size()
           << ", HE=" << m.get_half_edges().size()
           << ", F=" << m.get_faces().size() << ")";
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, std::shared_ptr<triMesh> const& p) {
        return p ? os << *p : os << "triMesh(nullptr)";
    }

} // namespace HalfMesh
