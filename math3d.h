#pragma once
#define _USE_MATH_DEFINES
#include <ostream>
#include <cmath>
#include <string>

struct Vec3 {
    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
    };

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


    Vec3& operator*=(const Vec3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vec3 operator-() const {
        return {-x, -y, -z};
    }

    bool operator==(Vec3 other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(Vec3 other) const {
        return x != other.x || y != other.y || z != other.z;
    }

    Vec3 operator*(const Vec3& other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator+(float scalar) const {
        return {x + scalar, y + scalar, z + scalar};
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

    Vec2i(int x, int y) : x(x), y(y) {}
};

struct Transform {
    Vec3 position;
    Vec3 rotation; // Euler angles in radians
    Vec3 scale;

    Transform() : position({0, 0, 0}), rotation({0, 0, 0}), scale({1, 1, 1}) {}
    Transform(Vec3 pos) : position(pos), rotation({0, 0, 0}), scale({1, 1, 1}) {}
    Transform(Vec3 pos, Vec3 rot) : position(pos), rotation(rot), scale({1, 1, 1}) {}
    Transform(Vec3 pos, Vec3 rot, Vec3 scale) : position(pos), rotation(rot), scale(scale) {}

    Vec3 forward() const;
    Vec3 right() const;
};


std::ostream& operator<<(std::ostream& os, const Vec3& v);
std::ostream& operator<<(std::ostream& os, const Vec2i& v);
std::istream& operator>>(std::istream& is, Vec3& v);
Vec3 operator+(const Vec3& a, const Vec3& b);
Vec3 operator-(Vec3 a, Vec3 b);


using Color = Vec3; // Alias for RGB color

namespace gmath {
    Vec3 cross(Vec3 a, Vec3 b);
    float dot(Vec3 a, Vec3 b);
    Vec3 normalize(Vec3 v);
    Vec3 rotateX(Vec3 v, float angle);
    Vec3 rotateY(Vec3 v, float angle);
    Vec3 rotateZ(Vec3 v, float angle);
    Vec3 reflect(Vec3 I, Vec3 normal);
}
