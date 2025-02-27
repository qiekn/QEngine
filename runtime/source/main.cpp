#include "game/register.h"

#include "ecs/registery.h"
#include "ecs/internal/entity.h"

#include "game/position.h"

#include "json/json.h"


int main() {
    Entity entity(1);

    Component position = Component::from_type<Position>(10,20);
    Registery::singleton().add_component(entity, std::move(position));

    const auto& pos = Registery::singleton().get_first_variant<Position>(entity);
    auto pos_str = json::to(pos, "test.variant");

    auto var = json::from(pos_str);
    std::cout << var.get_type().get_name() << std::endl;


    auto wrapped_value = var.get_wrapped_value<Position>();
    wrapped_value.print();

}
