#pragma once

#include <iostream>
#include <cassert>
#include <vector>

#include "core/entity.h"
#include "rttr/variant.h"
#include <filesystem>

class Zeytin {

public:
    Zeytin(const Zeytin&) = delete;
    Zeytin& operator=(const Zeytin&) = delete;

    Zeytin();

    std::string serialize_entity(const entity_id id);
    std::string serialize_entity(const entity_id id, const std::filesystem::path& path);
    
    void deserialize_entity(const std::filesystem::path& path);
    void deserialize_entity(const std::string& entity);

    void create_dummy(const rttr::type& type);

    entity_id new_entity_id();
    void add_variant(const entity_id&, rttr::variant);

    inline std::vector<rttr::variant>& get_variants(const entity_id& entity) {
        return m_storage[entity];
    }

    template<typename T, typename... Args>
    void add_variant(const entity_id& entity, Args&&... args) {
        T t(std::forward<Args>(args)...);
        m_storage[entity].push_back(std::move(t));
    }

    template<typename T>
    T& get_first(const entity_id& entity) {
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

    int m_entity_count = 0;
    std::unordered_map<entity_id, std::vector<rttr::variant>> m_storage;
};
