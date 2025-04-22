#pragma once

#include "variant/variant_base.h"

#include "game/position.h"

class Velocity : public VariantBase {
    VARIANT(Velocity);

public:
    float x = 0.0f; PROPERTY()
    float y = 0.0f; PROPERTY()
};
