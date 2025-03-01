#pragma once

#include <iostream>
#include <cassert>
#include <vector>

#include "core/internal/entity.h"
#include "rttr/variant.h"

using variants = std::vector<rttr::variant>;

class Scene {

public:
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    static Scene& singleton() {
        static Scene registery;
        return registery;
    }

    Entity new_entity();
    void add_variant(const entity_id&, rttr::variant);

    template<typename T, typename... Args>
    void add_variant(const entity_id& entity, Args&&... args) {
        assert(m_storage.find(entity) != m_storage.end());
        T t(std::forward<Args>(args)...);
        m_storage[entity].push_back(std::move(t));
    }

    inline variants& get_variants(const entity_id& entity) {
        assert(m_storage.find(entity) != m_storage.end());
        return m_storage[entity];
    }

    template<typename T>
    T& get_first(const entity_id& entity) {
        assert(m_storage.find(entity) != m_storage.end());
        const auto& rttr_type = rttr::type::get<T>();
        auto& variants = m_storage[entity];
        for(auto& variant : variants) {
            if(variant.get_type() == rttr_type) {
                return variant.get_value<T>();
            }
        }
    }

    template<typename T>
    const rttr::variant& get_first(const entity_id& entity) {
        assert(m_storage.find(entity) != m_storage.end());
        const auto& rttr_type = rttr::type::get<T>();
        auto& variants = m_storage[entity];
        for(auto& variant : variants) {
            if(variant.get_type() == rttr_type) {
                return variant;
            }
        }
    }


    // TODO: improve performance
    void tick_variants() {
        for(auto& pair : m_storage) {
            for(auto& variant : pair.second) {
                const rttr::type& type = variant.get_type();
                const rttr::method& method = type.get_method("Tick");

                if(method.is_valid()) {
                    method.invoke(variant);
                }
            }
        }
    }

private:
    Scene() {}

    std::unordered_map<entity_id, variants> m_storage;
};
