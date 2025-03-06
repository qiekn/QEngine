#pragma once

#include <vector>

#include "entity_document.h"

class EntityList final {

public:
    void load_entity_from_file(const std::filesystem::path&);
    void load_entities();
    void save_entities();
    
    inline std::vector<EntityDocument>& get_entities() { return m_entities;}

private:
    std::vector<EntityDocument> m_entities;
};
