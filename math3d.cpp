#include <iostream>
#include "math3d.h"

Vec3 operator+(const Vec3& a, const Vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 operator-(Vec3 a, Vec3 b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vec2i& v) {
    os << "Vec2i(" << v.x << ", " << v.y << ")";
    return os;
}

std::istream& operator>>(std::istream& is, Vec3& v) {
    char comma1, comma2;
    is >> v.x >> comma1 >> v.y >> comma2 >> v.z;
    if (comma1 != ',' || comma2 != ',') {
        is.setstate(std::ios::failbit);
    }
    return is;
}

namespace gmath {
    Vec3 cross(Vec3 a, Vec3 b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float dot(Vec3 a, Vec3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vec3 normalize(Vec3 v) {
        float length = std::sqrt(dot(v, v));
        if (length == 0) return {0, 0, 0};
        return {v.x / length, v.y / length, v.z / length};
    }

    Vec3 rotateX(Vec3 v, float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return {
            v.x,
            v.y * cosA - v.z * sinA,
            v.y * sinA + v.z * cosA
        };
    }

    Vec3 rotateY(Vec3 v, float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return {
            v.x * cosA + v.z * sinA,
            v.y,
            -v.x * sinA + v.z * cosA
        };
    }

    Vec3 rotateZ(Vec3 v, float angle) {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        return {
            v.x * cosA - v.y * sinA,
            v.x * sinA + v.y * cosA,
            v.z
        };
    }
}