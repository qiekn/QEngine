#include "game/register.h"

#include "ecs/registery.h"
#include "ecs/internal/entity.h"

#include "game/position.h"


int main() {
    Entity entity(1);

    Component position = Component::from_type<Position>(10,20);
    Registery::singleton().add_component(entity, std::move(position));

    const component_list& component_list = Registery::singleton().get_component_list(entity);

    const auto& pos = Registery::singleton().get_first<Position>(entity);
    pos.print();
}
