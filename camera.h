#pragma once
#include <cmath>
#include "math3d.h"

class Camera{
public:
    Vec3 position;
    Vec3 rotation;

    Camera(Vec3 pos) : position(pos), rotation({0, 0, 0}) {}
    Vec3 forward() const;
    Vec3 right() const {
        Vec3 fwd = forward();
        return {fwd.z, 0, -fwd.x}; // Simple right vector based on forward
    }
private:
    float m_Dir = 1.0f;
};