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

struct RotateComponent {
    float speed;
};

static AutoRegisterSchema<RotateComponent> regRotate({
    makeField(&RotateComponent::speed)
});

struct Material {
    Color color;
};

static AutoRegisterSchema<Material> regMaterial({
    makeField(&Material::color)
});

struct Collider {
    Vec3 size;
};

static AutoRegisterSchema<Collider> regCollider({
    makeField(&Collider::size)
});

struct Rigidbody {
    Vec3 velocity{0.0f, 0.0f, 0.0f};
    bool isStatic = false;
};

static AutoRegisterSchema<Rigidbody> regRigidbody({
    makeField(&Rigidbody::isStatic)
});