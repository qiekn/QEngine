#pragma once

#include "entity_document.h"

#include <vector>
#include <filesystem>


class EntityList final {
public:
    EntityList();
    inline std::vector<EntityDocument>& get_entities() { return m_entities; }
    
private:
    void register_event_handlers();
    void sync_entities_from_document(const rapidjson::Document& document);
    void backup_entities();
    void load_entity_from_file(const std::filesystem::path& path);
    void load_entities(const std::filesystem::path& path);
    void save_entities();

    std::vector<EntityDocument> m_entities;
};
