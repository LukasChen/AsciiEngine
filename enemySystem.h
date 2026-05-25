#pragma once
#include "registry.h"
#include "system.h"
#include "components_type.h"
#include "math3d.h"

class EnemySystem : public System<Enemy, Rigidbody, Transform> {
protected:
    using System::System;
    void update(View<Enemy, Rigidbody, Transform>& view, Registry&, float deltaTime) override;
};
