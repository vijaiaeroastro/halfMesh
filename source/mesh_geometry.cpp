#include "mesh.hpp"
#include <cmath>

namespace HalfMesh {

    double Mesh::get_area(unsigned fh) const {
        auto f = get_face(fh);
        auto [v1,v2,v3] = f->get_vertices();
        double x1=v1->get_x(), y1=v1->get_y(), z1=v1->get_z();
        double x2=v2->get_x(), y2=v2->get_y(), z2=v2->get_z();
        double x3=v3->get_x(), y3=v3->get_y(), z3=v3->get_z();

        double ux = x2-x1, uy = y2-y1, uz = z2-z1;
        double vx = x3-x1, vy = y3-y1, vz = z3-z1;
        double cx = uy*vz - uz*vy;
        double cy = uz*vx - ux*vz;
        double cz = ux*vy - uy*vx;
        return 0.5 * std::sqrt(cx*cx + cy*cy + cz*cz);
    }

    Vertex Mesh::get_face_normal(unsigned fh) const {
        auto f = get_face(fh);
        auto [v1,v2,v3] = f->get_vertices();
        double ux = v2->get_x() - v1->get_x();
        double uy = v2->get_y() - v1->get_y();
        double uz = v2->get_z() - v1->get_z();
        double vx = v3->get_x() - v1->get_x();
        double vy = v3->get_y() - v1->get_y();
        double vz = v3->get_z() - v1->get_z();
        double nx = uy*vz - uz*vy;
        double ny = uz*vx - ux*vz;
        double nz = ux*vy - uy*vx;
        return Vertex(nx,ny,nz);
    }

    double Mesh::get_face_angle(unsigned f1, unsigned f2) const {
        auto n1 = get_face_normal(f1);
        auto n2 = get_face_normal(f2);
        double dot = n1.get_x()*n2.get_x()
                   + n1.get_y()*n2.get_y()
                   + n1.get_z()*n2.get_z();
        double m1 = std::sqrt(n1.get_x()*n1.get_x()
                            + n1.get_y()*n1.get_y()
                            + n1.get_z()*n1.get_z());
        double m2 = std::sqrt(n2.get_x()*n2.get_x()
                            + n2.get_y()*n2.get_y()
                            + n2.get_z()*n2.get_z());
        return std::acos(dot/(m1*m2));
    }

} // namespace HalfMesh