#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include "game/register.h"
#include "core/zeytin.h"

int main() {
    Zeytin zeytin;

    int id = 10;


//    Position position(id);
//    position.x = 20;
//    position.y = 40;
//
//    zeytin.add_variant(id, position);
//    zeytin.serialize_entity(id, "osman.entity");
//
    std::filesystem::path path("osman.entity");
    zeytin.deserialize_entity(path);

    Position pos = zeytin.get_first<Position>(id);
    std::cout << pos.id <<std::endl;

    return 0;
}

