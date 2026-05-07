#pragma once
#include <vector>
#include <array>
#include <string>
#include "math3d.h"

class Model {
public:
    Model(const std::string& filename);
    int nverts() const;
    int nfaces() const;
    Vec3 vert(const int i) const;
    Vec3 vert(const int iface, const int nthvert) const;

private:
    std::vector<Vec3>                   m_Verts;
    std::vector<std::array<int, 3>>     m_Faces;
};