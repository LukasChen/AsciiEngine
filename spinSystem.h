#pragma once
#include "math3d.h"
#include "ecs.h"
#include "system.h"

class SpinSystem : public System<Transform> {
protected:
    void update(View<Transform>& view, float deltaTime) override {
        float speed = 3.0f;
        for (auto [transform] : view) {
            transform.rotation.y += deltaTime * speed; // Rotate around Y-axis
            transform.rotation.x += deltaTime * speed * 0.5f; // Rotate around X-axis
            transform.rotation.z += deltaTime * speed * 0.25f; // Rotate around Z-axis
        }
    }
};