#pragma once

#include <iostream>

#include "rttr/registration.h"
#include "core/entity.h"
#include "core/variant/variant_base.h"
#include "game/velocity.h"

struct Position : VariantBase {
    Position() = default;

    Position(VariantCreateInfo info) : VariantBase(info) {}

    void awake() override {
        std::cout << "----------------------------------AWAKE CALLED----------------" << std::endl;
        std::cout << get_id() << ": "<< "(" << x << "," << y << ")" << std::endl;
    }

    void tick() override {
        //std::cout << get_id() << ": "<< "(" << x << "," << y << ")" << std::endl;
    }

    int x = 0;
    int y = 0;

    RTTR_ENABLE(VariantBase);
};

