#pragma once
#include "math3d.h"
#include "components_type.h"
#include "system.h"

class RotateAnimSystem : public System<RotateComponent, Transform> {
protected:
    using System::System;

    void update(View<RotateComponent, Transform>& view, Registry&, float deltaTime) override {
        for (auto [rotateComp, transform] : view) {
            transform.rotation.x += rotateComp.speed * deltaTime;
        }
    }
};