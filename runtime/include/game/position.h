#pragma once

#include <iostream>

#include "rttr/registration.h"
#include "core/entity.h"
#include "core/variant/variant_base.h"
#include "game/velocity.h"

struct Position : VariantBase {
    Position() = default;

    Position(VariantCreateInfo info) : VariantBase(info) {}

    void print() {
        std::cout << get_id() << ": "<< "(" << x << "," << y << ")" << std::endl;
        entity_get_variant<Velocity>().print();
    }

    int x = 0;
    int y = 0;

    RTTR_ENABLE(VariantBase);
};

