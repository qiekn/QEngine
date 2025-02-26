#include <cassert>

#include "ecs/registery.h"

constexpr int RESERVED = 10;

void Registery::add_component(const entity_id& entity, Component component) {
    if(m_storage.find(entity) == m_storage.end()) {
        std::cout << "Entity is new: " << entity << "\n";
        m_storage[entity] = component_list();
        m_storage.reserve(RESERVED);
    }
    m_storage[entity].push_back(std::move(component));
}
