#pragma once

#include "variant/variant_base.h"

class Position : public VariantBase {
    VARIANT(Position)

public:
    Position(float x, float y) : x(x), y(y) {}

    float x = 0; PROPERTY() 
    float y = 0; PROPERTY();
};

