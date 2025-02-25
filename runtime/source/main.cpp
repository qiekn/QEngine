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

    auto& l = registery.query<Position>();
    for(auto& component : l) {
        auto& position = component.get<Position>();
        position.print();
    }
}
