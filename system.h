#pragma once
#include "ecs.h"
#include "view.h"

class BaseSystem {
public:
    virtual ~BaseSystem() = default;
    virtual void doStart(Registry& registry) = 0;
    virtual void doUpdate(Registry& registry, float deltaTime) = 0;
};

template<typename... Components>
class System : public BaseSystem {
public:
    void doStart(Registry& registry) {
        auto view = registry.view<Components...>();
        start(view);
    };
    void doUpdate(Registry& registry, float deltaTime) {
         auto view = registry.view<Components...>();
         update(view, deltaTime);
    };

protected:
    virtual void start(View<Components...>& /*view*/) {}
    virtual void update(View<Components...>& view, float deltaTime) = 0;
};
