#pragma once

#include <iostream>

#include "rttr/registration.h"
#include "core/entity.h"
#include "core/variant/base.h"

struct Position{

    int x = 0;
    int y = 0;


    void print() const {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }

};

