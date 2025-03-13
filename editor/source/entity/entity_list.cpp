#include "entity/entity_list.h"

#include <iostream>
#include <filesystem>

#include "rapidjson/document.h"
#include "engine/engine_event.h"


EntityList::EntityList() {
    load_entities();

    EngineEventBus::get().subscribe<const rapidjson::Document&>(
        EngineEvent::SyncEditor,
        [this](const rapidjson::Document& document) -> void {
            if (document.HasMember("entities") && document["entities"].IsArray()) {
                const auto& entities = document["entities"].GetArray();

                for (const auto& entity : entities) {
                    if (entity.HasMember("entity_id") && entity.HasMember("variants")) {
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
            }
        }
    );

}

void EntityList::load_entities() {
    m_entities.clear();

    for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(ENTITY_FOLDER)) {
        if(!entry.is_regular_file() || entry.path().extension() != ".entity") {
            continue;
        }

        std::filesystem::path file_path = entry.path();
        std::string file_name = file_path.stem().string();
        m_entities.emplace_back<EntityDocument>(std::move(file_name));
    }

    // NOTE: this could run parelel
    for(auto& entity : m_entities) {
        entity.load_from_file();
    }
}
void EntityList::load_entity_from_file(const std::filesystem::path& path) {
        std::string file_name = path.stem().string();
        auto& rv = m_entities.emplace_back<EntityDocument>(std::move(file_name));
        rv.load_from_file();
}

