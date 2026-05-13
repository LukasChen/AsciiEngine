#pragma once
#include "math3d.h"
#include "components_type.h"
#include "ecs.h"
#include "view.h"
#include "system.h"

const float GRAVITY = 9.81f;

struct AABB {
    Vec3 min;
    Vec3 max;
};

class PhysicsSystem: public System<Rigidbody, Collider, Transform> {
public:
    void update(View<Rigidbody, Collider, Transform>& view, float deltaTime);

private:
    bool checkCollision(const Transform& t1, const Transform& t2);
    AABB getAABB(const Transform& t);
    void resolveCollision(Transform& t1, Transform& t2);
};