#pragma once

#include "rttr/registration.h"
#include <iostream>

struct Position final {
    Position() {}

    Position(int in_x, int in_y) : x(in_x), y(in_y) {}

    Position(const Position& other) : x(other.x), y(other.y) {}

    int x = 0;
    int y = 0;

    Position* base = nullptr;


    void print() const {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }



    RTTR_ENABLE()
};

