#pragma once

#include <vector>
#include <strutil.hpp>

namespace HalfMesh {
    // Forward declarations
    class Vertex;
    class Face;
    class Edge;
    class HalfEdge;
    class Mesh;

    // --- Mesh I/O formats ---
    enum class MeshType {
        Gmsh       = 100,
        Stl        = 200,
        Binary     = 300,
        Vtk        = 500,
        Unknown    = 999
    };

    // --- Entity identifiers ---
    enum class EntityType {
        Vertex,
        Edge,
        Face,
        HalfEdge
    };

    // --- Property API return codes ---
    enum class PropertyStatus {
        Added,
        Exists,
        Deleted,
        CouldNotDelete,
        DoesNotExist,
        CouldNotAdd
    };

    // Utility to lower-case & detect extensions
    inline MeshType guess_mesh_format(const std::string& filename) {
        auto s = strutil::to_lower(filename);
        if (strutil::ends_with(s, ".msh")) return MeshType::Gmsh;
        if (strutil::ends_with(s, ".stl")) return MeshType::Stl;
        if (strutil::ends_with(s, ".bm" )) return MeshType::Binary;
        if (strutil::ends_with(s, ".vtk")) return MeshType::Vtk;
        return MeshType::Unknown;
    }

    // Helpers
    inline bool is_substring(const std::string& str, const std::string& sub) {
        return str.find(sub) != std::string::npos;
    }

    inline std::vector<std::string>
    split_string(const std::string& str, const std::string& delim, bool trim_empty = false) {
        std::vector<std::string> tokens;
        size_t start = 0, pos;
        while ((pos = str.find(delim, start)) != std::string::npos) {
            if (!trim_empty || pos > start)
                tokens.emplace_back(str.substr(start, pos - start));
            start = pos + delim.size();
        }
        if (!trim_empty || start < str.size())
            tokens.emplace_back(str.substr(start));
        return tokens;
    }

    template<typename T>
    constexpr T squared(T v) { return v * v; }
}