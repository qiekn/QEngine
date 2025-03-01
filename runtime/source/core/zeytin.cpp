#include <cassert>

#include "core/zeytin.h"

constexpr int RESERVED = 10;

void Zeytin::add_variant(const entity_id& entity, rttr::variant variant) {
    if(m_storage.find(entity) == m_storage.end()) {
        m_storage[entity] = std::vector<rttr::variant>();
        m_storage.reserve(RESERVED);
    }
    m_storage[entity].push_back(std::move(variant));
}

entity_id Zeytin::new_entity() {
    return ++m_entity_count;
}
