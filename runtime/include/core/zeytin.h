#pragma once

#include "raylib.h"

#include <vector>
#include <filesystem>
#include <optional>
#include <unordered_map>

#include "rapidjson/document.h"
#include "rttr/variant.h"

#include "entity/entity.h"
#include "editor/editor_communication.h"

#define get_zeytin() Zeytin::get()

class Zeytin {
public:
    Zeytin(const Zeytin&) = delete;
    Zeytin& operator=(const Zeytin&) = delete;
    static Zeytin& get() {
        static Zeytin instance;
        return instance;
    }

    void init();
    void run_frame();
    inline bool should_die() const { return m_should_die || WindowShouldClose(); }

    entity_id new_entity_id();
    
    void remove_variant(entity_id id, const rttr::type& type);
    void remove_entity(entity_id id);
    
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

    inline Camera2D& get_camera() { return m_camera; }
    inline const std::unordered_map<entity_id, std::vector<rttr::variant>>& get_storage() const { return m_storage; }
    inline std::unordered_map<entity_id, std::vector<rttr::variant>>& get_storage() { return m_storage; }

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

    bool m_synced_once = false;
#endif

private:
    Zeytin() = default;
    ~Zeytin() = default;
    
    void initialize_camera();
    void update_camera();
    
    bool m_post_inited = false;
    bool m_started = false;
    bool m_should_die = false;

    bool m_is_scene_ready = false;
    bool m_is_play_mode = false;
    bool m_is_pause_play_mode = false;

    std::unordered_map<entity_id, std::vector<rttr::variant>> m_storage;

    // NOTE: maybe move these to somewhere else
    RenderTexture2D m_render_texture;
    Vector2 m_render_position;
    Camera2D m_camera;

#ifdef EDITOR_MODE
    std::unique_ptr<EditorCommunication> m_editor_communication;
#endif
};
