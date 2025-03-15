#include "entity/entity_list.h"

#include <filesystem>
#include "rapidjson/document.h"

#include "engine/engine_event.h"

namespace {
    constexpr const char* BACKUP_DIR = "temp_backup";
}

EntityList::EntityList() {
    load_entities(ENTITY_FOLDER);
    register_event_handlers();
}

void EntityList::register_event_handlers() {
    EngineEventBus::get().subscribe<const rapidjson::Document&>(
        EngineEvent::SyncEditor,
        [this](const rapidjson::Document& document) {
            sync_entities_from_document(document);
        }
    );
    
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EnterPlayMode,
        [this](auto) {
            backup_entities();
        }
    );
    
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::ExitPlayMode,
        [this](bool) {
            load_entities(BACKUP_DIR);
        }
    );
}

void EntityList::sync_entities_from_document(const rapidjson::Document& document) {
    if (!document.HasMember("entities") || !document["entities"].IsArray()) {
        return;
    }
    
    const auto& entities = document["entities"].GetArray();
    for (const auto& entity : entities) {
        if (!entity.HasMember("entity_id") || !entity.HasMember("variants")) {
            continue;
        }
        
        uint64_t entityId = entity["entity_id"].GetUint64();
        bool found = false;
        
        for (auto& entityDoc : m_entities) {
            const auto& existingDoc = entityDoc.get_document();
            if (existingDoc.HasMember("entity_id") &&
                existingDoc["entity_id"].GetUint64() == entityId) {
                rapidjson::Document newDoc;
                newDoc.CopyFrom(entity, newDoc.GetAllocator());
                entityDoc.set_document(std::move(newDoc));
                found = true;
                break;
            }
        }
    }
}

void EntityList::backup_entities() {
    std::filesystem::path backupDir = BACKUP_DIR;
    
    if (!std::filesystem::exists(backupDir)) {
        std::filesystem::create_directory(backupDir);
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDir)) {
        std::filesystem::remove(entry.path());
    }
    
    for (const auto& entity : m_entities) {
        std::string name = entity.get_name();
        std::filesystem::path backupPath = backupDir / (name + ".entity");
        entity.save_to_file(backupPath);
    }
}

void EntityList::load_entities(const std::filesystem::path& path) {
    m_entities.clear();
    
    if (!std::filesystem::exists(path)) {
        return;
    }
    
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".entity") {
            continue;
        }
        
        std::filesystem::path file_path = entry.path();
        std::string file_name = file_path.stem().string();
        
        auto& entity = m_entities.emplace_back(EntityDocument(std::move(file_name)));
        entity.load_from_file();
    }
}

void EntityList::load_entity_from_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        return;
    }
    
    std::string file_name = path.stem().string();
    auto& entity = m_entities.emplace_back(EntityDocument(std::move(file_name)));
    entity.load_from_file();
}
