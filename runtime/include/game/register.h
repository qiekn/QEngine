#include "position.h"
#include "player.h"
#include "core/internal/entity.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Position>("Position")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<int,int>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y)
        .method("Tick", &Position::print);
}
