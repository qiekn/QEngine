#include "game/register.h"

#include "ecs/registery.h"
#include "ecs/internal/entity.h"

#include "game/position.h"

#include "json/json.h"


int main() {
    json::create_dummy(rttr::type::get_by_name("Entity"));
}
