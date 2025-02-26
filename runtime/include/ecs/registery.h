#pragma once

#include <iostream>
#include <cassert>

#include "ecs/internal/component.h"
#include "ecs/internal/entity.h"

using component_list = std::vector<Component>;

class Registery {

public:
    Registery(const Registery&) = delete;
    Registery& operator=(const Registery&) = delete;

    static Registery& singleton() {
        static Registery registery;
        return registery;
    }

    void add_component(const entity_id&, Component);

    inline component_list& get_component_list(const entity_id& entity) {
        assert(m_storage.find(entity) != m_storage.end());
        return m_storage[entity];
    }

    template<typename T>
    T& get_first(const entity_id& entity) {
        assert(m_storage.find(entity) != m_storage.end());
        const auto& rttr_type = rttr::type::get<T>();
        auto& component_list = m_storage[entity];
        for(auto& component : component_list) {
            if(component.var.get_type() == rttr_type) {
                return component.get<T>();
            }
        }
    }

private:
    Registery() {}

    std::unordered_map<entity_id, component_list> m_storage;
};
