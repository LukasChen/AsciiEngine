#pragma once
#include <vector>
#include <array>
#include <string>
#include "math3d.h"

class Model {
public:
    Model(const std::string& filename);
    Model(std::vector<Vec3> verts, std::vector<std::array<int, 3>> faces, std::vector<Vec3> normals) 
        : m_Verts(std::move(verts)), m_Faces(std::move(faces)), m_Normals(std::move(normals)) {}
    int nverts() const;
    int nfaces() const;
    Vec3 vert(const int i) const;
    Vec3 vert(const int iface, const int nthvert) const;
    Vec3 normal(const int i) const;
    Vec3 normal(const int iface, const int nthvert) const;
private:
    std::vector<Vec3>                   m_Verts;
    std::vector<std::array<int, 3>>     m_Faces;
    std::vector<Vec3>                   m_Normals;
};