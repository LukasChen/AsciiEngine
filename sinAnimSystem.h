#pragma once
#include "math3d.h"
#include "components_type.h"
#include "system.h"

class SinAnimSystem : public System<SinComponent, Transform> {
protected:
    void start(View<SinComponent, Transform>& view) override {
        for (auto [sinComp, transform] : view) {
            sinComp.startY = transform.position.y;
            sinComp.time = 0.0f;
        }
    }
    void update(View<SinComponent, Transform>& view, float deltaTime) override {
        for (auto [sinComp, transform] : view) {
            transform.position.y = sinComp.startY + std::sin(sinComp.time * 5) * 0.5f;
            sinComp.time += deltaTime;
        }
    }
};