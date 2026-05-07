#include "camera.h"

void Camera::pingpong() {
    position.z += 0.005f * m_Dir;
    if (position.z > -0.5f) {
        m_Dir = -1.0f;
    } else if (position.z < -1.0f) {
        m_Dir = 1.0f;
    }
}

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