#pragma once
#include "ecs.h"
#include "view.h"

class BaseSystem {
public:
    BaseSystem(Registry& registry) : m_registry(registry) {}
    virtual ~BaseSystem() = default;
    virtual void doStart(Registry& registry) = 0;
    virtual void doUpdate(Registry& registry, float deltaTime) = 0;
protected:
    Registry& m_registry;
};

template<typename... Components>
class System : public BaseSystem {
public:
    System(Registry& registry) : BaseSystem(registry) {}

    void doStart(Registry& registry) {
        auto view = registry.view<Components...>();
        start(view);
        registry.flushDeferredAdds();
    };
    void doUpdate(Registry& registry, float deltaTime) {
         auto view = registry.view<Components...>();
         update(view, registry, deltaTime);
         registry.flushDeferredAdds();
    };

protected:
    virtual void start(View<Components...>& /*view*/) {}

    virtual void update(View<Components...>& view, Registry& registry, float deltaTime) = 0;

    template<typename T>
    void AddComponent(Entity entity, T&& component) {
        m_registry.get<T>().deferAddComponent(entity, std::forward<T>(component));
    }

    void DestroyEntity(Entity entity) {
        m_registry.deferDestroy(entity);
    }
};
