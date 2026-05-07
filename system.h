#pragma once
#include "ecs.h"
#include "scene.h"

template<typename... Components>
class System {
public:
    virtual void start(Scene& scene) {}
    void update(Scene& scene, float deltaTime) {
         
    };
};

