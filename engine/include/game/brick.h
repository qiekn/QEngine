#pragma once

#include "variant/variant_base.h"
#include "game/collider.h"

class Brick : public VariantBase {
    VARIANT(Brick);

public:
    int health = 1; PROPERTY()
    Color color = RED; PROPERTY()
    bool destroyed = false; PROPERTY()
};
