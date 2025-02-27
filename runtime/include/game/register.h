#pragma once

#include "position.h"
#include "player.h"
#include "ecs/internal/entity.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{

    rttr::registration::class_<Position>("Position")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<int,int>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y)
        .property("base", &Position::base);
        

    rttr::registration::class_<Player>("Player")
        .constructor<>();

    rttr::registration::class_<Entity>("Entity")
        .constructor<>()
        .property("id", &Entity::id);
}



