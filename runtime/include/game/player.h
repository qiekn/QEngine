#pragma once

#include "rttr/registration.h"

#include "position.h"
#include "velocity.h"

struct NamedInt {
    std::string name;
    int value;
};


struct Player final {
    std::string display_name;

    double health = 0;

    Position position;
    Velocity velocity;

    RTTR_ENABLE()
};
