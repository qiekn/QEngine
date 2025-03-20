#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>
#include <optional>
#include <unordered_map>

#include "rapidjson/document.h"
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

    entity_id new_entity_id();
    void add_variant(const entity_id& entity, rttr::variant variant);
    
    template<typename T, typename... Args>
    void add_variant(const entity_id& entity, Args&&... args) {
        T t(std::forward<Args>(args)...);
        m_storage[entity].push_back(std::move(t));
    }
    
    template<typename T>
    std::optional<std::reference_wrapper<T>> try_get_variant(const entity_id entity_id) {
        const rttr::type& rttr_type = rttr::type::get<T>();
        auto entity_it = m_storage.find(entity_id);
        if (entity_it == m_storage.end()) {
            return std::nullopt;
        }
        
        auto& variants = entity_it->second;
        for (auto& variant : variants) {
            if (variant.get_type() == rttr_type) {
                return std::reference_wrapper<T>(variant.get_value<T>());
            }
        }
        
        return std::nullopt;
    }

    void remove_variant(entity_id id, const rttr::type& type);

    void remove_entity(entity_id id) {
        get_variants(id).clear();
    }
    
    void clean_dead_variants();
    std::vector<rttr::variant>& get_variants(const entity_id& entity);

    std::string serialize_entity(const entity_id id);
    std::string serialize_entity(const entity_id id, const std::filesystem::path& path);
    entity_id deserialize_entity(const std::string& entity);
    entity_id deserialize_entity(const std::filesystem::path& path);
    
    std::string serialize_scene();
    void deserialize_scene(const std::string& scene);
    void deserialize_entities();

    void post_init_variants();
    void update_variants();
    void play_start_variants();
    void play_update_variants();

#ifdef EDITOR_MODE
    void generate_variants();
    void generate_variant(const rttr::type& type);
    void subscribe_editor_events();
    void sync_editor();
    
    void enter_play_mode(bool is_paused);
    void exit_play_mode();
    void pause_play_mode();
    
    void handle_entity_property_changed(const rapidjson::Document& doc);
    void handle_entity_variant_added(const rapidjson::Document& msg);
    void handle_entity_variant_removed(const rapidjson::Document& msg);
    void handle_entity_removed(const rapidjson::Document& msg);
    
    inline bool is_play_mode() const { return m_is_play_mode; }
    inline bool is_paused_play_mode() const { return m_is_pause_play_mode; }
    inline bool is_scene_ready() const { return m_is_scene_ready; }
    inline bool should_die() const { return m_should_die; }

    bool m_synced_once = false;
#endif

private:
    Zeytin() = default;
    ~Zeytin() = default;
    
    bool m_post_inited = false;
    bool m_started = false;
    std::unordered_map<entity_id, std::vector<rttr::variant>> m_storage;

#ifdef EDITOR_MODE
    bool m_is_scene_ready = false;
    bool m_is_play_mode = false;
    bool m_is_pause_play_mode = false;
    bool m_should_die = false;

#endif
};
