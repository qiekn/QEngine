#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>

#include "rttr/type.h"
#include "core/entity.h"


class Zeytin {      
public:
    Zeytin(const Zeytin&) = delete; 
    Zeytin& operator=(const Zeytin&) = delete;

    static Zeytin& get() {
        static Zeytin instance;
        return instance;
    }

    void generate_variants();
    void init();
              
    std::string serialize_entity(const entity_id id);
    std::string serialize_entity(const entity_id id, const std::filesystem::path& path);
    
    entity_id deserialize_entity(const std::filesystem::path& path);
    entity_id deserialize_entity(const std::string& entity);

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
    void try_get_variant(const entity_id entity_id, rttr::variant& out_variant) {
        const auto& rttr_type = rttr::type::get<T>();
        auto& variants = m_storage[entity_id];
        for(auto& variant : variants) {
            if(variant.get_type() == rttr_type) {
                out_variant = variant;
            }
        }
    }

    void update_variants();

    void play_start_variants();
    void play_update_variants();

private:
    Zeytin() = default;
    ~Zeytin() = default;
    
    void create_dummy(const rttr::type& type);

    std::unordered_map<entity_id, std::vector<rttr::variant>> m_storage;
};
