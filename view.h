#pragma once
#include "ecs.h"

template<typename... Components>
class View {
public:
    View(ComponentArr<Components>&... componentsArrs) : m_componentArrs(componentsArrs...) {}

    class Iterator {
    public:
        Iterator(View* view, size_t index) : m_view(view), m_index(index) {
            skipInvalid();
        }

        auto operator*() {
            Entity e = m_view->primary().entityToIndex.at(m_index);

            return std::tuple<Components&...>(
                std::get<ComponentArr<Components>&>(
                    m_view->m_componentArrs
                ).get(e)...
            );
        }

        Iterator& operator++() {
            m_index++;
            skipInvalid();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return m_index != other.m_index;
        }

    private:
        View* m_view;
        size_t m_index;

        void skipInvalid() {
            while (m_index < m_view->primary().entityToIndex.size()) {
                Entity e = m_view->primary().entityToIndex.at(m_index);
                if (m_view->containsAll(e)) {
                    break;
                }
                m_index++;
            }
        }
    };

    Iterator begin() {
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, primary().entityToIndex.size());
    }
private:
    std::tuple<ComponentArr<Components>&...> m_componentArrs;

    auto& primary() {
        return std::get<0>(m_componentArrs);
    }

    bool containsAll(Entity entity) const {
        return (std::get<ComponentArr<Components>&>(m_componentArrs).has(entity) && ...);
    }
};
