#pragma once
#include "ecs.h"
#include "view.h"
#include "components_type.h"
#include "input.h"
#include "primitive.h"

class CharacterMovementSystem : public System<Transform, Camera> {
public:
    using System::System;

    void update(View<Transform, Camera>& view, Registry& registry, float deltaTime) override {
        for (auto [transform, camera] : view) {
            transform.position += 
                gmath::normalize(transform.forward()) * (Input::GetAxis(Axis::Vertical) * camera.moveSpeed) * deltaTime; // Move speed
            transform.position += 
                gmath::normalize(transform.right()) * (Input::GetAxis(Axis::Horizontal) * camera.moveSpeed) * deltaTime; // Move speed
            transform.rotation.y += Input::GetAxis(Axis::LookHorizontal) * camera.lookSpeed * deltaTime; // Look speed


            if (Input::isKeyPressed(VK_SPACE)) {
                Entity bulletEntity = registry.create();

                AddComponent<Transform>(bulletEntity, Transform(transform.position, Vec3{0,0,0}, Vec3{0.1f, 0.1f, 0.1f}));
                AddComponent<Rigidbody>(bulletEntity, Rigidbody{transform.forward() * 5.0f, Vec3{0, 0, 0}});
                AddComponent<Collider>(bulletEntity, Collider{Vec3{0.1f, 0.1f, 0.1f}});
                AddComponent<Material>(bulletEntity, Material(Color{1, 0, 0}));
                AddComponent<Model>(bulletEntity, Primitive::createSphere(8));
            }
        }
    }
};
