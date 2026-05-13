#include "primitive.h"

Model Primitive::createPlane(){
    return Model(
        {
            {-1, 0, -1},
            { 1, 0, -1},
            { 1, 0,  1},
            {-1, 0,  1}
        }, 
        {
            {{{0, 2, 1}, {0, 0, 0}}},
            {{{0, 3, 2}, {0, 0, 0}}}
        }, 
        {
            {0, 1, 0}
        }
    );
}

Model Primitive::createSphere(int segments) {
    std::vector<Vec3> verts;
    std::vector<std::array<std::array<int, 3>, 2>> faces;
    std::vector<Vec3> normals;

    for (int i = 0; i <= segments; ++i) {
        float theta = i * M_PI / segments;
        for (int j = 0; j <= segments; ++j) {
            float phi = j * 2 * M_PI / segments;
            Vec3 vert{
                std::sin(theta) * std::cos(phi),
                std::cos(theta),
                std::sin(theta) * std::sin(phi)
            };
            verts.push_back(vert);
            normals.push_back(gmath::normalize(vert));
        }
    }

    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int idx1 = i * (segments + 1) + j;
            int idx2 = idx1 + segments + 1;

            faces.push_back({{{idx1, idx2, idx1 + 1}, {idx1, idx2, idx1 + 1}}});
            faces.push_back({{{idx1 + 1, idx2, idx2 + 1}, {idx1 + 1, idx2, idx2 + 1}}});
        }
    }

    return Model(verts, faces, normals);
}

Model Primitive::createCylinder(int segments) {
    std::vector<Vec3> verts;
    std::vector<std::array<std::array<int, 3>, 2>> faces;
    std::vector<Vec3> normals;

    // Create vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2 * M_PI / segments;
        float x = std::cos(angle);
        float z = std::sin(angle);
        verts.push_back({x, -0.5f, z}); // Bottom circle
        verts.push_back({x, 0.5f, z});  // Top circle
        normals.push_back({x, 0, z});   // Normal for side vertices
    }
    verts.push_back({0, -0.5f, 0}); // Center of bottom circle
    verts.push_back({0, 0.5f, 0});  // Center of top circle
    normals.push_back({0, -1, 0});   // Normal for bottom center
    normals.push_back({0, 1, 0});    // Normal for top center

    int centerBottomIdx = segments * 2;
    int centerTopIdx = segments * 2 + 1;

    // Create faces
    for (int i = 0; i < segments; ++i) {
        int idx1 = i * 2;
        int idx2 = idx1 + 1;
        int idx3 = ((i + 1) % segments) * 2;
        int idx4 = idx3 + 1;

        // Side faces
        faces.push_back({{{idx1, idx3, idx2}, {idx1, idx3, idx2}}});
        faces.push_back({{{idx2, idx3, idx4}, {idx2, idx3, idx4}}});

        // Bottom face
        faces.push_back({{{centerBottomIdx, idx1, idx3}, {centerBottomIdx, idx1, idx3}}});

        // Top face
        faces.push_back({{{centerTopIdx, idx4, idx2}, {centerTopIdx, idx4, idx2}}});
    }

    return Model(verts, faces, normals);
}