#include "position.h"
#include "player.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Position>("Position")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<int,int>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y)
        .method("Tick", &Position::print);

    rttr::registration::class_<Velocity>("Velocity")
        .constructor<>()
        .property("x", &Velocity::x)
        .property("y", &Velocity::y);

    rttr::registration::class_<Player>("Player")
        .constructor<>()
        .property("position", &Player::position)
        .property("velocity", &Player::velocity);
}



