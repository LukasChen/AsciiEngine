#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const std::string& filename) {

    std::cout << "Loading model from file: " << filename << std::endl; // flush immediately
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::cout << "Parsing model data...\n";

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vec3 v = {0, 0, 0};
            if (iss >> v.x >> v.y >> v.z) {
                m_Verts.push_back(v);
            }
        } else if (prefix == "f") {
            std::array<int, 3> face = {0, 0, 0};
            int i = 0;
            std::string token;
            while (iss >> token && i < 3) {
                face[i++] = std::stoi(token.substr(0, token.find('/'))) - 1;
            }
            if (i == 3) m_Faces.push_back(face);
        }
    }
    std::cout << "Loaded " << m_Verts.size() << " verts and " << m_Faces.size() << " faces.\n";
}

int Model::nverts() const { return (int)m_Verts.size(); }
int Model::nfaces() const { return (int)m_Faces.size(); }

Vec3 Model::vert(int i) const {
    return m_Verts[i];
}

Vec3 Model::vert(int iface, int nthvert) const {
    return m_Verts[m_Faces[iface][nthvert]];
}
