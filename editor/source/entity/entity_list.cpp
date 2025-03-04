#include "entity/entity_list.h"

#include <filesystem>

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
