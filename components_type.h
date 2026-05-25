#pragma once
#include "math3d.h"
#include "schemaRegistry.h"
#include "ecs.h"

struct SinComponent {
    float speed;
    float amplitude;
    float offset;
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

    Material() {}
    Material(Color c) : color(c) {}
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
    Vec3 angularVelocity{0.0f, 0.0f, 0.0f};
    bool isStatic = false;
    bool useGravity = false;
};

static AutoRegisterSchema<Rigidbody> regRigidbody({
    makeField(&Rigidbody::isStatic),
    makeField(&Rigidbody::useGravity)
});

struct Camera {
    float moveSpeed = 5.0f;
    float lookSpeed = 30.0f;
};

struct Enemy {
    float speed;
};

static AutoRegisterSchema<Enemy> regEnemy({
    makeField(&Enemy::speed)
});

struct Bullet {};