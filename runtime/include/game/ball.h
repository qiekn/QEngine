#pragma once

#include "variant/variant_base.h"

class Ball : public VariantBase {
    VARIANT(Ball)

public:
    float radius = 10.0f; PROPERTY()
    bool active = true; PROPERTY()
};
