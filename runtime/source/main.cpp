#include "game/register.h" // IWYU pragma: keep

#include "core/entity.h"
#include "core/zeytin.h"
#include "core/json/json.h"

int main() {
    //Entity entity(1);
    //Position position(2,4);

    //Zeytin::singleton().add_variant(entity, position);
    //Zeytin::singleton().add_variant<Position>(entity,50,60);

    //const std::vector<rttr::variant>& variants = Zeytin::singleton().get_variants(entity);

    //std::cout << zeytin::json::serialize_entity(entity.get_id(), variants, "test.entity") << std::endl;

    entity_id id;
    std::vector<rttr::variant> variants;
    variants.reserve(10);

    std::filesystem::path path = "test.entity";
    
    zeytin::json::deserialize_entity(path, id, variants);

    std::cout << id << std::endl;

    for(const auto& variant : variants) {
        variant.get_value<Position>().print();
    }

}
