#include "camera.h"

Vec3 Camera::forward() const {
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