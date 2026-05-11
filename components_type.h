#pragma once
#include "math3d.h"
#include "schemaRegistry.h"

struct SinComponent {
    float speed;
    float amplitude;
    float startY;
    float time;
};

static AutoRegisterSchema<SinComponent> regSin({
    makeField(&SinComponent::speed),
    makeField(&SinComponent::amplitude)
});

struct Material {
    Color color;
};

static AutoRegisterSchema<Material> regMaterial({
    makeField(&Material::color)
});