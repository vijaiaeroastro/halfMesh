#include "mesh.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

namespace HalfMesh {

void Mesh::save(const std::string& fn) const {
    auto type = guess_mesh_format(fn);
    if      (type==MeshType::Gmsh)   write_gmsh(fn);
    else if (type==MeshType::Obj)    write_obj(fn);
    else if (type==MeshType::Binary) write_binary(fn);
    else if (type==MeshType::Vtk)    write_vtk(fn);
    else std::cerr<<"Unknown format: "<<fn<<"\n";
}

void Mesh::read(const std::string& fn) {
    auto type = guess_mesh_format(fn);
    if      (type==MeshType::Gmsh)   read_gmsh(fn);
    else if (type==MeshType::Obj)    read_obj(fn);
    else if (type==MeshType::Binary) read_binary(fn);
    else std::cerr<<"Unknown format: "<<fn<<"\n";
    complete_mesh();
}

// — Gmsh —
void Mesh::read_gmsh(const std::string& fn) {
    clear_data();
    std::ifstream in(fn);
    std::string line;
    bool in_nodes=false, in_elems=false;
    std::unordered_map<unsigned,VertexPtr> tmp;
    while(std::getline(in,line)) {
        if (line=="$Nodes")      { in_nodes=true;  continue; }
        if (line=="$EndNodes")   { in_nodes=false; continue; }
        if (line=="$Elements")   { in_elems=true;  continue; }
        if (line=="$EndElements"){ in_elems=false; continue; }
        if (in_nodes) {
            unsigned idx; double x,y,z;
            std::istringstream iss(line);
            if(iss>>idx>>x>>y>>z)
                tmp[idx]=add_vertex(x,y,z);
        }
        if (in_elems) {
            std::istringstream iss(line);
            unsigned idx,type; if(!(iss>>idx>>type)) continue;
            if(type!=2) continue;
            unsigned nt; iss>>nt; for(unsigned k=0;k<nt;++k) iss>>idx;
            unsigned n1,n2,n3; iss>>n1>>n2>>n3;
            add_face(tmp[n1], tmp[n2], tmp[n3]);
        }
    }
}

void Mesh::read_obj(const std::string& fn) {
    clear_data();
    std::ifstream in(fn);
    std::string line;
    std::vector<VertexPtr> vtx;
    while(std::getline(in,line)) {
        if (line.rfind("v ",0)==0) {
            std::istringstream iss(line.substr(2));
            double x,y,z; if(iss>>x>>y>>z)
                vtx.push_back(add_vertex(x,y,z));
        }
        if (line.rfind("f ",0)==0) {
            std::istringstream iss(line.substr(2));
            unsigned i1,i2,i3; char slash;
            if((iss>>i1>>slash>>slash>>i2>>slash>>slash>>i3)
             ||(iss>>i1>>i2>>i3))
            {
                add_face(vtx[i1-1], vtx[i2-1], vtx[i3-1]);
            }
        }
    }
}

void Mesh::read_binary(const std::string& fn) {
    clear_data();
    std::ifstream in(fn, std::ios::binary);
    std::vector<uint8_t> buf((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());
    auto js = nlohmann::json::from_bson(buf);
    for(auto& vv: js["VERTICES"])
        add_vertex(vv[0], vv[1], vv[2]);
    for(auto& ff: js["FACES"])
        add_face(
          get_vertex(ff[0].get<unsigned>()),
          get_vertex(ff[1].get<unsigned>()),
          get_vertex(ff[2].get<unsigned>()));
    vertex_data_store = js["VERTEX_PROPERTIES"];
    edge_data_store   = js["EDGE_PROPERTIES"];
    face_data_store   = js["FACE_PROPERTIES"];
}

// — Writers —
void Mesh::write_gmsh(const std::string& fn) const {
    std::ofstream out(fn);
    out<<"$MeshFormat\n2.2 0 "<<sizeof(double)<<"\n$EndMeshFormat\n";
    out<<"$Nodes\n"<<vertices_.size()<<"\n";
    for(auto& v: vertices_)
        out<<v->handle()+1<<" "<<v->get_x()<<" "<<v->get_y()<<" "
           <<v->get_z()<<"\n";
    out<<"$EndNodes\n$Elements\n"<<faces_.size()<<"\n";
    for(auto& f: faces_) {
        auto [a,b,c] = f->get_vertices();
        out<<f->handle()+1<<" 2 2 0 1 "
           <<a->handle()+1<<" "<<b->handle()+1<<" "<<c->handle()+1<<"\n";
    }
    out<<"$EndElements\n";
}

void Mesh::write_obj(const std::string& fn) const {
    std::ofstream out(fn);
    for(auto& v: vertices_)
        out<<"v "<<v->get_x()<<" "<<v->get_y()<<" "<<v->get_z()<<"\n";
    for(auto& f: faces_) {
        auto [a,b,c] = f->get_vertices();
        out<<"f "<<a->handle()+1<<" "<<b->handle()+1<<" "<<c->handle()+1<<"\n";
    }
}

void Mesh::write_binary(const std::string& fn) const {
    nlohmann::json js;
    for(auto& v: vertices_)
        js["VERTICES"].push_back({v->get_x(),v->get_y(),v->get_z()});
    for(auto& f: faces_) {
        auto [a,b,c] = f->get_vertices();
        js["FACES"].push_back({a->handle(),b->handle(),c->handle()});
    }
    js["VERTEX_PROPERTIES"] = vertex_data_store;
    js["EDGE_PROPERTIES"  ] = edge_data_store;
    js["FACE_PROPERTIES"  ] = face_data_store;
    auto buf = nlohmann::json::to_bson(js);
    std::ofstream out(fn, std::ios::binary);
    out.write(reinterpret_cast<char*>(buf.data()), buf.size());
}

void Mesh::write_vtk(const std::string& fn) const {
    std::ofstream out(fn);
    out<<"# vtk DataFile Version 2.0\nHalfMesh VTK\nASCII\nDATASET POLYDATA\n";
    out<<"POINTS "<<vertices_.size()<<" float\n";
    for(auto& v: vertices_)
        out<<v->get_x()<<" "<<v->get_y()<<" "<<v->get_z()<<"\n";
    out<<"POLYGONS "<<faces_.size()<<" "<<faces_.size()*4<<"\n";
    for(auto& f: faces_) {
        auto [a,b,c] = f->get_vertices();
        out<<"3 "<<a->handle()<<" "<<b->handle()<<" "<<c->handle()<<"\n";
    }
}

} // namespace HalfMesh