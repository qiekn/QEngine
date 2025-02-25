#include <cassert>

#include "ecs/registery.h"
#include "ecs/internal/component_store.h"

Registery::Registery() {
    const auto& all_types = rttr::type::get_types();
    component_store.update_component_types(all_types);
}
