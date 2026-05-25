#pragma once
#include "system.h"

class BulletSystem : public System<Bullet>  {
public:
    using System::System;
protected:
    void update(View<Bullet>& view, Registry&, float) {
        for (auto& collisionEvent : m_registry.collisionEvents) {
            if (collisionEvent.a->hasComponent<Bullet>() && collisionEvent.b->hasComponent<Enemy>()) {

                m_registry.destroy(collisionEvent.a);

            }
        }
    }
}