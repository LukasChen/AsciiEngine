#pragma once
#define _USE_MATH_DEFINES
#include <ostream>
#include <cmath>
#include <string>

struct Vec3 {
    float x, y, z;

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    std::string toString() const {
        return "Vec3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

struct Vec2 {
    float x, y;
};

struct Vec2i {
    int x, y;
};

struct Transform {
    Vec3 position;
    Vec3 rotation; // Euler angles in radians
    Vec3 scale;

    Transform() : position({0, 0, 0}), rotation({0, 0, 0}), scale({1, 1, 1}) {}
};

std::ostream& operator<<(std::ostream& os, const Vec3& v);
std::ostream& operator<<(std::ostream& os, const Vec2i& v);
Vec3 operator+(const Vec3& a, const Vec3& b);
Vec3 operator-(Vec3 a, Vec3 b);

namespace gmath {
    Vec3 cross(Vec3 a, Vec3 b);
    float dot(Vec3 a, Vec3 b);
    Vec3 normalize(Vec3 v);
    Vec3 rotateX(Vec3 v, float angle);
    Vec3 rotateY(Vec3 v, float angle);
    Vec3 rotateZ(Vec3 v, float angle);
}
