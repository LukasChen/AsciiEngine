#include "physicSystem.h"

namespace {
    constexpr float SPIN_SCALE = 0.04f;
    constexpr float SPIN_DAMPING = 0.995f;
}

void PhysicsSystem::update(View<Rigidbody, Collider, Transform>& view, float deltaTime) {
    for (auto [rb, col, tr] : view) {

        rb.velocity.y -= GRAVITY * deltaTime;
        tr.position += rb.velocity * deltaTime;
        tr.rotation += rb.angularVelocity * deltaTime;
        rb.angularVelocity = rb.angularVelocity * SPIN_DAMPING;

        AABB aabb = getAABB(tr, col);
        // Simple ground collision
        if (aabb.min.y < 0.0f) {
            tr.position.y -= aabb.min.y; // Move up to ground level
            rb.velocity.y = 0.0f; // Stop vertical velocity
            rb.angularVelocity += gmath::cross(Vec3{0.0f, 1.0f, 0.0f}, rb.velocity) * SPIN_SCALE;
        }
    }

    for (auto first = view.begin(); first != view.end(); ++first) {
        auto [rb1, col1, tr1] = *first;

        auto second = first;
        ++second;
        for (; second != view.end(); ++second) {
            auto [rb2, col2, tr2] = *second;

            if (checkCollision(tr1, col1, tr2, col2)) {
                resolveCollision(rb1, tr1, col1, rb2, tr2, col2);
            }
        }
    }
}

bool PhysicsSystem::checkCollision(const Transform& t1, const Collider& col1, const Transform& t2, const Collider& col2) {
    AABB aabb1 = getAABB(t1, col1);
    AABB aabb2 = getAABB(t2, col2);

    return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
           (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
           (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);
}

AABB PhysicsSystem::getAABB(const Transform& t, const Collider& col) {
    Vec3 halfSize = col.size * 0.5f;
    AABB aabb;
    aabb.min = t.position - halfSize;
    aabb.max = t.position + halfSize;
    return aabb;
}

Vec3 PhysicsSystem::getPenetration(const AABB& aabb1, const AABB& aabb2) {
    float xOverlap = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
    float yOverlap = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);
    float zOverlap = std::min(aabb1.max.z, aabb2.max.z) - std::max(aabb1.min.z, aabb2.min.z);

    return Vec3{xOverlap, yOverlap, zOverlap};
}

void PhysicsSystem::applySpin(Rigidbody& rb1, Rigidbody& rb2, const Vec3& collisionNormal) {
    Vec3 relativeVelocity = rb1.velocity - rb2.velocity;
    Vec3 tangentVelocity = relativeVelocity - collisionNormal * gmath::dot(relativeVelocity, collisionNormal);
    Vec3 spin = gmath::cross(collisionNormal, tangentVelocity);

    float spinStrength = std::sqrt(gmath::dot(tangentVelocity, tangentVelocity));
    if (spinStrength < 0.0001f) {
        return;
    }

    Vec3 angularDelta = spin * (SPIN_SCALE * spinStrength);

    if (!rb1.isStatic) {
        rb1.angularVelocity += angularDelta;
    }
    if (!rb2.isStatic) {
        rb2.angularVelocity -= angularDelta;
    }
}

void PhysicsSystem::resolveCollision(Rigidbody& rb1, Transform& t1, Collider& col1, Rigidbody& rb2, Transform& t2, Collider& col2) {
    AABB a = getAABB(t1, col1);
    AABB b = getAABB(t2, col2);

    Vec3 pen = getPenetration(a, b);

    if (rb1.isStatic && rb2.isStatic) {
        return;
    }

    if (pen.x <= pen.y && pen.x <= pen.z) {
        float dir = (a.min.x + a.max.x) < (b.min.x + b.max.x) ? -1.0f : 1.0f;
        float move = pen.x;
        Vec3 normal = Vec3{dir, 0.0f, 0.0f};

        if (rb1.isStatic) {
            t2.position.x -= move * dir;
        } else if (rb2.isStatic) {
            t1.position.x += move * dir;
        } else {
            float splitMove = move * 0.5f;
            t1.position.x += splitMove * dir;
            t2.position.x -= splitMove * dir;
        }

        applySpin(rb1, rb2, normal);

    } else if (pen.y <= pen.x && pen.y <= pen.z) {
        float dir = (a.min.y + a.max.y) < (b.min.y + b.max.y) ? -1.0f : 1.0f;
        float move = pen.y;
        Vec3 normal = Vec3{0.0f, dir, 0.0f};

        if (rb1.isStatic) {
            t2.position.y -= move * dir;
            rb2.velocity.y = 0.0f;
        } else if (rb2.isStatic) {
            t1.position.y += move * dir;
            rb1.velocity.y = 0.0f;
        } else {
            float splitMove = move * 0.5f;
            t1.position.y += splitMove * dir;
            t2.position.y -= splitMove * dir;
            rb1.velocity.y = 0.0f;
            rb2.velocity.y = 0.0f;
        }

        applySpin(rb1, rb2, normal);

    } else {
        float dir = (a.min.z + a.max.z) < (b.min.z + b.max.z) ? -1.0f : 1.0f;
        float move = pen.z;
        Vec3 normal = Vec3{0.0f, 0.0f, dir};

        if (rb1.isStatic) {
            t2.position.z -= move * dir;
        } else if (rb2.isStatic) {
            t1.position.z += move * dir;
        } else {
            float splitMove = move * 0.5f;
            t1.position.z += splitMove * dir;
            t2.position.z -= splitMove * dir;
        }

        applySpin(rb1, rb2, normal);

    }
}