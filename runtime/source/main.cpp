#include "game/register.h" // IWYU pragma: keep

#include "core/internal/entity.h"
#include "core/scene.h"
#include "core/json/json.h"


int main() {
    Entity entity(1);
    Position position(2,4);

    Scene::singleton().add_variant(entity, position);
    Scene::singleton().add_variant<Position>(entity,50,60);

    auto& variants = Scene::singleton().get_variants(entity);


    //Scene::singleton().tick_variants();

    std::cout << json::to(1) << std::endl;
}
