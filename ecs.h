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

    // Pass-by-value and move idiom
    void addComponent(Entity entity, T component) {
        m_entityToIndex[entity] = data.size();
        m_indexToEntity.push_back(entity);
        data.push_back(std::move(component));
    }

    // perfect forward
    template<typename... Args>
    void emplaceComponent(Entity entity, Args&&... args) {
        m_entityToIndex[entity] = data.size();
        m_indexToEntity.push_back(entity);
        data.emplace_back(std::forward<Args>(args)...);
    }

    bool has(Entity entity) const {
        return m_entityToIndex.find(entity) != m_entityToIndex.end();
    }

    T& get(Entity entity) {
        return data[m_entityToIndex.at(entity)];
    }

    T* tryGet(Entity entity) {
        auto it = m_entityToIndex.find(entity);
        if (it != m_entityToIndex.end()) {
            return &data[it->second];
        }
        return nullptr;
    }

    Entity getEntity(size_t index) const {
        return m_indexToEntity[index];
    }

private:
    std::unordered_map<Entity, size_t> m_entityToIndex;
    std::vector<Entity> m_indexToEntity;
};
