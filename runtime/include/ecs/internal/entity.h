#pragma once

#include <cstdint>

using Entity = uint16_t;
static uint32_t entity_count = 0;

namespace entity {
    Entity new_entity();

    template<class T>
        bool add_component(const Entity entity) {

        }

}
