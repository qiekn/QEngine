#pragma once

#include <cstdint>
#include <string>

using entity_id = uint16_t;

class Entity final {

public:
    inline Entity() {}
    inline Entity(entity_id id) : id(id) {}

    inline entity_id get_id() const { return id; }

#if DEBUG
    std::string name;
#endif

    inline operator entity_id() const { return id; }

private:
    entity_id id;
};
