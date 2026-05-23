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

Vec3 Transform::forward() const {
    // Compute forward vector based on rotation
    float cosPitch = std::cos(rotation.x);
    float sinPitch = std::sin(rotation.x);
    float cosYaw = std::cos(rotation.y);
    float sinYaw = std::sin(rotation.y);

    return {
        cosPitch * sinYaw,
        -sinPitch,
        cosPitch * cosYaw
    };
}

Vec3 Transform::right() const {
    Vec3 fwd = forward();
    return {fwd.z, 0, -fwd.x}; // Simple right vector based on forward
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

    Vec3 reflect(Vec3 I, Vec3 normal) {
        float d = dot(I, normal);
        return I - normal * d * 2.0f;
    }

    Color hsvToRgb(float h, float s, float v) {
        auto clamp01 = [](float value) {
            return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
        };

        h = std::fmod(h, 360.0f);
        if (h < 0.0f) h += 360.0f;
        s = clamp01(s);
        v = clamp01(v);

        float c = v * s;
        float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        Color rgb{0.0f, 0.0f, 0.0f};
        if (h < 60.0f) {
            rgb = {c, x, 0.0f};
        } else if (h < 120.0f) {
            rgb = {x, c, 0.0f};
        } else if (h < 180.0f) {
            rgb = {0.0f, c, x};
        } else if (h < 240.0f) {
            rgb = {0.0f, x, c};
        } else if (h < 300.0f) {
            rgb = {x, 0.0f, c};
        } else {
            rgb = {c, 0.0f, x};
        }

        return rgb + m;
    }

    Color hsvToRgb(Vec3 hsv) {
        return hsvToRgb(hsv.x, hsv.y, hsv.z);
    }
}