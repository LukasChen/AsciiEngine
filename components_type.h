#pragma once
#include "math3d.h"
#include "schemaRegistry.h"

struct SinComponent {
    float startY;
    float time;
};

static AutoRegisterSchema<SinComponent> regSin({
    makeField(&SinComponent::startY),
    makeField(&SinComponent::time)
});

struct Material {
    Color color;
};

static AutoRegisterSchema<Material> regMaterial({
    makeField(&Material::color)
});