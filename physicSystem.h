#pragma once
#include "math3d.h"
#include "components_type.h"
#include "ecs.h"
#include "view.h"
#include "system.h"
#include "logger.h"

const float GRAVITY = 9.81f;

struct AABB {
    Vec3 min;
    Vec3 max;
};

class PhysicsSystem: public System<Rigidbody, Collider, Transform> {
public:
    void start(View<Rigidbody, Collider, Transform>& view) override {
        // Initialize rigidbodies with some velocity
        for (auto [rb, col, tr] : view) {
            rb.velocity = Vec3{0, 0, 0};
            rb.angularVelocity = Vec3{0, 0, 0};
        }
    }
    void update(View<Rigidbody, Collider, Transform>& view, float deltaTime);

private:
    bool checkCollision(const Transform& t1, const Collider& col1, const Transform& t2, const Collider& col2);
    AABB getAABB(const Transform& t, const Collider& col);
    Vec3 getPenetration(const AABB& aabb1, const AABB& aabb2);
    void applySpin(Rigidbody& rb1, Rigidbody& rb2, const Vec3& collisionNormal);
    void resolveCollision(Rigidbody& rb1, Transform& t1, Collider& col1, Rigidbody& rb2, Transform& t2, Collider& col2);
};
