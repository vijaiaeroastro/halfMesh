#include "triMesh.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

namespace halfMesh {
    void triMesh::save(const std::string &fn) const {
        switch (guess_mesh_format(fn)) {
            case MeshType::Gmsh:
                write_gmsh(fn);
                break;
            case MeshType::Stl:
                write_stl_binary(fn);
                break;
            case MeshType::Binary:
                write_binary(fn);
                break;
            case MeshType::Vtk:
                write_vtk(fn);
                break;
            default:
                std::cerr << "Unknown format: " << fn << "\n";
                break;
        }
    }

    void triMesh::read(const std::string &filename) {
        switch (guess_mesh_format(filename)) {
            case MeshType::Gmsh:
                read_gmsh(filename);
                break;
            case MeshType::Stl:
                read_stl(filename);
                break;
            case MeshType::Binary:
                read_binary(filename);
                break;
            default:
                std::cerr << "Unknown format: " << filename << std::endl;
        }
    }

    // — Gmsh —
    void triMesh::read_gmsh(const std::string &fn) {
        clear_data();
        std::ifstream in(fn);
        std::string line;
        bool in_nodes = false, in_elems = false;
        std::unordered_map<unsigned, vertexPtr> tmp;
        while (std::getline(in, line)) {
            if (line == "$Nodes") {
                in_nodes = true;
                continue;
            }
            if (line == "$EndNodes") {
                in_nodes = false;
                continue;
            }
            if (line == "$Elements") {
                in_elems = true;
                continue;
            }
            if (line == "$EndElements") {
                in_elems = false;
                continue;
            }
            if (in_nodes) {
                unsigned idx;
                double x, y, z;
                std::istringstream iss(line);
                if (iss >> idx >> x >> y >> z)
                    tmp[idx] = add_vertex(x, y, z);
            }
            if (in_elems) {
                std::istringstream iss(line);
                unsigned idx, type;
                if (!(iss >> idx >> type)) continue;
                if (type != 2) continue;
                unsigned nt;
                iss >> nt;
                for (unsigned k = 0; k < nt; ++k) iss >> idx;
                unsigned n1, n2, n3;
                iss >> n1 >> n2 >> n3;
                add_face(tmp[n1], tmp[n2], tmp[n3]);
            }
        }
        complete_mesh();
    }

    void triMesh::read_binary(const std::string &fn) {
        clear_data();
        std::ifstream in(fn, std::ios::binary);
        std::vector<uint8_t> buf((std::istreambuf_iterator<char>(in)),
                                 std::istreambuf_iterator<char>());
        auto js = nlohmann::json::from_bson(buf);
        for (auto &vv: js["VERTICES"])
            add_vertex(vv[0], vv[1], vv[2]);
        for (auto &ff: js["FACES"])
            add_face(
                get_vertex(ff[0].get<unsigned>()),
                get_vertex(ff[1].get<unsigned>()),
                get_vertex(ff[2].get<unsigned>()));
        vertex_data_store = js["VERTEX_PROPERTIES"];
        edge_data_store = js["EDGE_PROPERTIES"];
        face_data_store = js["FACE_PROPERTIES"];
        complete_mesh();
    }

    // — Writers —
    void triMesh::write_gmsh(const std::string &fn) const {
        std::ofstream out(fn);
        out << "$MeshFormat\n2.2 0 " << sizeof(double) << "\n$EndMeshFormat\n";
        out << "$Nodes\n" << vertices_.size() << "\n";
        for (auto &v: vertices_)
            out << v->handle() + 1 << " " << v->get_x() << " " << v->get_y() << " "
                    << v->get_z() << "\n";
        out << "$EndNodes\n$Elements\n" << faces_.size() << "\n";
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            out << f->handle() + 1 << " 2 2 0 1 "
                    << a->handle() + 1 << " " << b->handle() + 1 << " " << c->handle() + 1 << "\n";
        }
        out << "$EndElements\n";
    }

    void triMesh::write_obj(const std::string &fn) const {
        std::ofstream out(fn);
        for (auto &v: vertices_)
            out << "v " << v->get_x() << " " << v->get_y() << " " << v->get_z() << "\n";
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            out << "f " << a->handle() + 1 << " " << b->handle() + 1 << " " << c->handle() + 1 << "\n";
        }
    }

    void triMesh::write_binary(const std::string &fn) const {
        nlohmann::json js;
        for (auto &v: vertices_)
            js["VERTICES"].push_back({v->get_x(), v->get_y(), v->get_z()});
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            js["FACES"].push_back({a->handle(), b->handle(), c->handle()});
        }
        js["VERTEX_PROPERTIES"] = vertex_data_store;
        js["EDGE_PROPERTIES"]   = edge_data_store;
        js["FACE_PROPERTIES"]   = face_data_store;

        auto buf = nlohmann::json::to_bson(js);
        std::ofstream out(fn, std::ios::binary);
        out.write(
            reinterpret_cast<const char*>(buf.data()),
            static_cast<std::streamsize>(buf.size())
        );
    }

    void triMesh::write_vtk(const std::string &fn) const {
        std::ofstream out(fn);
        out << "# vtk DataFile Version 2.0\nHalfMesh VTK\nASCII\nDATASET POLYDATA\n";
        out << "POINTS " << vertices_.size() << " float\n";
        for (auto &v: vertices_)
            out << v->get_x() << " " << v->get_y() << " " << v->get_z() << "\n";
        out << "POLYGONS " << faces_.size() << " " << faces_.size() * 4 << "\n";
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            out << "3 " << a->handle() << " " << b->handle() << " " << c->handle() << "\n";
        }
    }

    //
    // Write ASCII STL
    //
    void triMesh::write_stl_ascii(const std::string &fn) const {
        std::ofstream out(fn);
        if (!out) {
            std::cerr << "Can't open " << fn << "\n";
            return;
        }
        out << "solid halfMesh\n";
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            // normal
            vertex n = get_face_normal(f->handle());
            double nx = n.get_x(), ny = n.get_y(), nz = n.get_z();
            double len = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 0) {
                nx /= len;
                ny /= len;
                nz /= len;
            }
            out << "  facet normal " << nx << " " << ny << " " << nz << "\n";
            out << "    outer loop\n";
            out << "      vertex " << a->get_x() << " " << a->get_y() << " " << a->get_z() << "\n";
            out << "      vertex " << b->get_x() << " " << b->get_y() << " " << b->get_z() << "\n";
            out << "      vertex " << c->get_x() << " " << c->get_y() << " " << c->get_z() << "\n";
            out << "    endloop\n";
            out << "  endfacet\n";
        }
        out << "endsolid halfMesh\n";
    }

    //
    // Write binary STL
    //
    void triMesh::write_stl_binary(const std::string &fn) const {
        std::ofstream out(fn, std::ios::binary);
        if (!out) {
            std::cerr << "Can't open " << fn << "\n";
            return;
        }
        // 80-byte header
        char header[80] = {};
        std::strncpy(header, "HalfMesh binary STL", sizeof(header));
        out.write(header, 80);
        // tri count
        auto count = static_cast<uint32_t>(faces_.size());
        out.write(reinterpret_cast<char *>(&count), 4);
        // each tri
        for (auto &f: faces_) {
            auto [a,b,c] = f->get_vertices();
            // normal
            vertex n = get_face_normal(f->handle());
            double nx = n.get_x(), ny = n.get_y(), nz = n.get_z();
            if (double len = std::sqrt(nx * nx + ny * ny + nz * nz); len > 0) {
                nx /= len;
                ny /= len;
                nz /= len;
            }
            out.write(reinterpret_cast<char *>(&nx), 4);
            out.write(reinterpret_cast<char *>(&ny), 4);
            out.write(reinterpret_cast<char *>(&nz), 4);
            // vertices
            auto writev = [&](const vertexPtr &v) {
                double x = v->get_x(), y = v->get_y(), z = v->get_z();
                out.write(reinterpret_cast<char *>(&x), 4);
                out.write(reinterpret_cast<char *>(&y), 4);
                out.write(reinterpret_cast<char *>(&z), 4);
            };
            writev(a);
            writev(b);
            writev(c);
            // attribute bytes
            uint16_t attr = 0;
            out.write(reinterpret_cast<char *>(&attr), 2);
        }
    }

    //
    // Read STL (auto–detect ASCII vs binary)
    //
    void triMesh::read_stl(const std::string &fn) {
        // peek first 512 bytes
        std::ifstream in(fn, std::ios::binary);
        if (!in) {
            std::cerr << "Can't open " << fn << "\n";
            return;
        }
        char buf[512] = {};
        in.read(buf, sizeof(buf));
        std::string head(buf, static_cast<size_t>(in.gcount()));
        in.close();

        // in the first 512 bytes, assume ASCII
        if (head.rfind("solid", 0) == 0 && head.find("facet") != std::string::npos) {
            read_stl_ascii(fn);
        } else {
            read_stl_binary(fn);
        }
        complete_mesh();
    }

    //
    // Read ASCII STL
    //
    void triMesh::read_stl_ascii(const std::string &fn) {
        clear_data();
        std::ifstream in(fn);
        if (!in) {
            std::cerr << "Can't open " << fn << "\n";
            return;
        }

        // temporary storage of unique vertices
        std::map<std::tuple<double, double, double>, vertexPtr> vmap;
        std::string line;
        std::vector<vertexPtr> tri;

        auto parse_vertex_line = [&](const std::string &l) {
            std::istringstream iss(l);
            std::string tmp;
            double x, y, z;
            if (!(iss >> tmp >> x >> y >> z)) return vertexPtr();
            auto key = std::make_tuple(x, y, z);
            auto it = vmap.find(key);
            if (it != vmap.end()) return it->second;
            auto v = add_vertex(x, y, z);
            vmap[key] = v;
            return v;
        };

        while (std::getline(in, line)) {
            // trim leading spaces
            auto p = line.find_first_not_of(" \t");
            if (p != std::string::npos) line = line.substr(p);

            if (line.rfind("vertex ", 0) == 0) {
                if (auto v = parse_vertex_line(line)) tri.push_back(v);
            } else if (line.rfind("endfacet", 0) == 0) {
                if (tri.size() == 3) add_face(tri[0], tri[1], tri[2]);
                tri.clear();
            }
        }
    }

    //
    // Read binary STL
    //
    void triMesh::read_stl_binary(const std::string &fn) {
        clear_data();
        std::ifstream in(fn, std::ios::binary);
        if (!in) {
            std::cerr << "Can't open " << fn << "\n";
            return;
        }
        // header
        char header[80];
        in.read(header, 80);
        uint32_t triCount;
        in.read(reinterpret_cast<char *>(&triCount), 4);

        // map to dedupe
        std::map<std::tuple<double, double, double>, vertexPtr> vmap;

        auto get_vertex = [&](double x, double y, double z) {
            auto key = std::make_tuple(x, y, z);
            auto it = vmap.find(key);
            if (it != vmap.end()) return it->second;
            auto v = add_vertex(x, y, z);
            vmap[key] = v;
            return v;
        };

        for (uint32_t i = 0; i < triCount; ++i) {
            double nx, ny, nz;
            in.read(reinterpret_cast<char *>(&nx), 4);
            in.read(reinterpret_cast<char *>(&ny), 4);
            in.read(reinterpret_cast<char *>(&nz), 4);
            std::array<vertexPtr, 3> tri;
            for (int k = 0; k < 3; ++k) {
                double x, y, z;
                in.read(reinterpret_cast<char *>(&x), 4);
                in.read(reinterpret_cast<char *>(&y), 4);
                in.read(reinterpret_cast<char *>(&z), 4);
                tri[k] = get_vertex(x, y, z);
            }
            // skip attribute bytes
            in.seekg(2, std::ios::cur);
            add_face(tri[0], tri[1], tri[2]);
        }
    }
} // namespace HalfMesh
