#pragma once

#include "variant/variant_base.h"

class Position : public VariantBase {
    VARIANT(Position)

public:
    float x = 0; PROPERTY()
    float y = 0; PROPERTY()
};

