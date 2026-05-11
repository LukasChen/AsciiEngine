#pragma once
#include "math3d.h"
#include "components_type.h"
#include "system.h"

class RotateAnimSystem : public System<RotateComponent, Transform> {
protected:
    void update(View<RotateComponent, Transform>& view, float deltaTime) override {
        for (auto [rotateComp, transform] : view) {
            transform.rotation.x += rotateComp.speed * deltaTime;
        }
    }
};