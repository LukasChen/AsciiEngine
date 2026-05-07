#include <cstdint>
#include <vector>
#include <unordered_map>
#pragma once
using Entity = uint32_t; // Simple alias for entity IDs

class Registry{
public:
    Entity create() {
        return m_nextEntity++;
    }

    Entity getEntiityCount() const {
        return m_nextEntity;
    }
private:
    Entity m_nextEntity = 0;
};

template<typename T>
class ComponentArr {
public:
    std::vector<T> data;
    std::unordered_map<Entity, size_t> entityToIndex;

    // Pass-by-value and move idiom
    void addComponent(Entity entity, T component) {
        entityToIndex[entity] = data.size();
        data.push_back(std::move(component));
    }

    // perfect forward
    template<typename... Args>
    void emplaceComponent(Entity entity, Args&&... args) {
        entityToIndex[entity] = data.size();
        data.emplace_back(std::forward<Args>(args)...);
    }

    T* getComponent(Entity entity) {
        auto it = entityToIndex.find(entity);
        if (it != entityToIndex.end()) {
            return &data[it->second];
        }
        return nullptr;
    }
};