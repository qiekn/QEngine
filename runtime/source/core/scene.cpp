#include <cassert>

#include "core/scene.h"

constexpr int RESERVED = 10;

void Scene::add_variant(const entity_id& entity, rttr::variant variant) {
    if(m_storage.find(entity) == m_storage.end()) {
        m_storage[entity] = variants();
        m_storage.reserve(RESERVED);
    }
    m_storage[entity].push_back(std::move(variant));
}

entity_id Scene::new_entity() {
    return ++m_entity_count;
}
