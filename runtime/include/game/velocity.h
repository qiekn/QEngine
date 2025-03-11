#pragma once

#include "rttr/registration.h"
#include "core/variant/variant_base.h"

class Velocity : public VariantBase {
    VARIANT(Velocity)

public:
    int x = 0; PROPERTY()
    int y = 0; PROPERTY()

    void tick() {
        std::cout << get_id() << ": "<< "(" << x << "," << y << ")" << std::endl;
    }
};
