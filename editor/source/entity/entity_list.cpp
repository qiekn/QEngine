#include "entity/entity_list.h"

#include <filesystem>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "engine/engine_event.h"

namespace {
    constexpr const char* BACKUP_DIR = "temp_backup";
}

EntityList::EntityList() {
    load_entities(ENTITY_FOLDER);
    register_event_handlers();
}

void EntityList::register_event_handlers() {
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EngineStarted,
        [this](auto _) {
            std::string scene = as_string();
            EngineEventBus::get().publish<const std::string&>(EngineEvent::EngineSendScene, scene);
        }
    );

    EngineEventBus::get().subscribe<const rapidjson::Document&>(
        EngineEvent::SyncEditor,
        [this](const rapidjson::Document& document) {
            if(m_should_sync_runtime) {
                sync_entities_from_document(document);
            }
        }
    );
    
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EnterPlayMode,
        [this](auto) {
            m_should_sync_runtime = true;
            backup_entities();
        }
    );
    
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::ExitPlayMode,
        [this](bool) {
            m_should_sync_runtime = false;
            load_entities(BACKUP_DIR);
        }
    );
}

std::string EntityList::as_string() const {
    rapidjson::Document document;
    document.SetObject();
    auto& allocator = document.GetAllocator();

    document.AddMember("type", "scene", allocator);
    
    rapidjson::Value entities_array(rapidjson::kArrayType);
    
    for (const auto& entity : m_entities) {
        if (entity.is_dead()) {
            continue;
        }
        
        std::string entity_str = entity.as_string();
        
        rapidjson::Document entity_doc;
        entity_doc.Parse(entity_str.c_str());
        
        if (!entity_doc.HasParseError()) {
            rapidjson::Value entity_value;
            entity_value.CopyFrom(entity_doc, allocator);
            entities_array.PushBack(entity_value, allocator);
        }
    }
    
    document.AddMember("entities", entities_array, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    
    return std::string(buffer.GetString(), buffer.GetSize());
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
        
        uint64_t entity_id = entity["entity_id"].GetUint64();
        bool found = false;
        
        for (auto& entity_doc : m_entities) {
            const auto& existing_doc = entity_doc.get_document();
            if (existing_doc.HasMember("entity_id") &&
                existing_doc["entity_id"].GetUint64() == entity_id) {
                rapidjson::Document new_doc;
                new_doc.CopyFrom(entity, new_doc.GetAllocator());
                entity_doc.set_document(std::move(new_doc));
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
        entity.load_from_file(file_path);
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
