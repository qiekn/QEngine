#include "ecs/internal/entity.h"

Entity entity::new_entity() {
    return ++entity_count;
}
