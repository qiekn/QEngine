#pragma once

#include "rttr/registration.h"
#include "core/variant/variant_base.h"

struct Velocity : VariantBase {
    Velocity() = default;
    Velocity(VariantCreateInfo info) : VariantBase(info) {}

    int x = 0;
    int y = 0;

    void tick() {
        std::cout << get_id() << ": "<< "(" << x << "," << y << ")" << std::endl;
    }

    RTTR_ENABLE(VariantBase)
};
