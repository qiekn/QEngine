#pragma once

#include "variant/variant_base.h"

class Velocity : public VariantBase {
    VARIANT(Velocity)

public:
    float x = 0.0f; PROPERTY()
    float y = 0.0f; PROPERTY()
};
