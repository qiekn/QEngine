#pragma once

#include "rttr/registration.h"

#include "position.h"
#include "velocity.h"

struct Player final {
    Position position;
    Velocity velocity;
    RTTR_ENABLE()
};
