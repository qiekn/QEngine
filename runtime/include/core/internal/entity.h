#pragma once

#include <cstdint>
#include <string>

using entity_id = uint16_t;

class Entity final {

public:
    Entity() {}

    inline Entity(entity_id id) : id(id) {}

    entity_id id;

#if DEBUG
    std::string name;
#endif

    operator entity_id() const {
        return id;
    }
};
