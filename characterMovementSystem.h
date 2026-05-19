#pragma once
#include "ecs.h"
#include "view.h"
#include "components_type.h"
#include "input.h"

class CharacterMovementSystem : public System<Transform, Camera> {
public:
    void update(View<Transform, Camera>& view, float deltaTime) override {
        for (auto [transform, camera] : view) {
            transform.position += 
                gmath::normalize(transform.forward()) * (Input::GetAxis(Axis::Vertical) * camera.moveSpeed) * deltaTime; // Move speed
            transform.position += 
                gmath::normalize(transform.right()) * (Input::GetAxis(Axis::Horizontal) * camera.moveSpeed) * deltaTime; // Move speed
            transform.rotation.y += Input::GetAxis(Axis::LookHorizontal) * camera.lookSpeed * deltaTime; // Look speed
        }
    }
};
