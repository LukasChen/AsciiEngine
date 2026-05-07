#pragma once
#include <utility>
#include "math3d.h"
#include "model.h"

class GameObject {
public:
    Transform transform;

    GameObject(Model&& model) : m_model(std::move(model)) {}
    GameObject(Model&& model, Transform&& transform) : transform(std::move(transform)), m_model(std::move(model)) {}

    const Model& getModel() const { return m_model; }
private:
    Model m_model;
};