#pragma once
#include "math3d.h"
#include "ecs.h"
#include "model.h"

class Scene {
public:
    // Registry registry;
    ComponentArr<Transform> transforms;
    ComponentArr<Model> models;
};