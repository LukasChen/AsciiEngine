#pragma once
#include "logger.h"
#include "math3d.h"
#include "components_type.h"
#include "system.h"

class SinAnimSystem : public System<SinComponent, Transform> {
protected:
    using System::System;

    void start(View<SinComponent, Transform>& view) override {
        for (auto [sinComp, transform] : view) {
            sinComp.startY = transform.position.y;
            sinComp.time = 0.0f;
            Logger::log("start offset " + std::to_string(sinComp.offset));
        }
    }
    void update(View<SinComponent, Transform>& view, Registry&, float deltaTime) override {
        for (auto [sinComp, transform] : view) {
            transform.position.y = sinComp.startY + std::sin((sinComp.time * sinComp.speed) + sinComp.offset) * sinComp.amplitude;
            sinComp.time += deltaTime;
        }
    }
};
