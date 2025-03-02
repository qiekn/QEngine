#pragma once

#include "rttr/registration.h"

struct Velocity final {
    int x = 0;
    int y = 0;

    RTTR_ENABLE()
};
