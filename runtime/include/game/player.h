#pragma once

#include "rttr/registration.h"
#include "core/variant/variant_base.h"

#include "position.h"
#include "velocity.h"


struct Player final {
    std::string display_name;

    double health = 0;

    Position position;
    Velocity velocity;

    RTTR_ENABLE()
};
