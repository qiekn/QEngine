#include "game/register.h"

#include "ecs/registery.h"
#include "ecs/internal/entity.h"
#include "ecs/internal/component_store.h"

#include "game/position.h"


int main() {
    Registery registery;
    
    Entity entity(1);

    registery.add_component<Position>(entity, 10, 20);
    registery.add_component<Position>(entity, 30, 40);
    registery.add_component<Position>(entity, 60, 70);

    auto&& [position1, position2] = registery.query<Position, Position>();


    for(auto& position_component : position1) {
        auto& position = position_component.get<Position>();
        position.x = 31;
        position.y = 31;
    }

    for(const auto& position_component : position2) {
        const auto& position = position_component.get<Position>();
        position.print();
    }

}
