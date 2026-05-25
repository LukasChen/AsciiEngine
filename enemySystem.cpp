#include "enemySystem.h"
#include "logger.h"

void EnemySystem::update(View<Enemy, Rigidbody, Transform>& view, Registry&, float deltaTime) {
    Transform& player = m_registry.get<Transform>().get(m_registry.GetPlayerEntity());

    for (auto& [enemy, rb, transform] : view) {
        Vec3 dir = gmath::normalize(player.position - transform.position);
        rb.velocity = dir * enemy.speed;
    }
}
