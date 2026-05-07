#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <typeindex>
using Entity = uint32_t; // Simple alias for entity IDs

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
};

template<typename T>
class ComponentArr : public IComponentArray {
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

    bool has(Entity entity) const {
        return entityToIndex.find(entity) != entityToIndex.end();
    }

    T& get(Entity entity) {
        return data[entityToIndex.at(entity)];
    }

    T* tryGet(Entity entity) {
        auto it = entityToIndex.find(entity);
        if (it != entityToIndex.end()) {
            return &data[it->second];
        }
        return nullptr;
    }
};
