#pragma once
#include <unordered_map>
#include <memory>
#include "ecs.h"
#include "view.h"

class Registry{
public:
    Entity create() {
        return m_nextEntity++;
    }

    Entity getEntiityCount() const {
        return m_nextEntity;
    }

    template<typename T>
    ComponentArr<T>& get() {
        std::type_index typeIdx(typeid(T));
        if (componentArrays.find(typeIdx) == componentArrays.end()) {
            componentArrays[typeIdx] = std::make_unique<ComponentArr<T>>();
        }
        return *static_cast<ComponentArr<T>*>(componentArrays[typeIdx].get());
    }

    template<typename... Components>
    View<Components...> view() {
        return View<Components...>(get<Components>()...);
    }

private:
    Entity m_nextEntity = 0;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentArrays;
};