#pragma once

#include <iostream>

#include "rttr/registration.h"
#include "core/entity.h"
#include "core/variant/variant_base.h"

struct Position : IVariantBase {
    Position(VariantCreateInfo info) : IVariantBase(entity_id) {}

    int x = 0;
    int y = 0;


    void print() const {
        std::cout << "(" << x << "," << y << ")" << std::endl;
    }

    RTTR_ENABLE(IVariantBase);
};

