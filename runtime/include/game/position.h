#pragma once

#include "rttr/registration.h"
#include <iostream>

struct Position final {
    Position() {}

    Position(int in_x, int in_y) : x(in_x), y(in_y) {}

    int x;
    int y;

    void print() const {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }

    RTTR_ENABLE()
};
