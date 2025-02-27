#pragma once

#include "position.h"
#include "player.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{

    rttr::registration::class_<Position>("Position")
        .constructor<>()
        .constructor<int,int>()
        .property("x", &Position::x)
        .property("y", &Position::y);

    rttr::registration::class_<Player>("Player");
}



