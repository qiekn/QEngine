#pragma once

#include <cstdint>

using entity_id = uint16_t;

#include "ecs/registery.h"


class Entity final {

public:
    inline Entity(entity_id id) : id(id) {}

    entity_id id;

    operator entity_id() const {
        return id;
    }
};
