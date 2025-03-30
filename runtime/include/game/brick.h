#pragma once

#include "variant/variant_base.h"
#include "game/collider.h"

class Brick : public VariantBase {
    VARIANT(Brick);
    REQUIRES(Collider); // collider already requires Position

public:
    int health = 1; PROPERTY()
    Color color = RED; PROPERTY()
    bool destroyed = false; PROPERTY()
    
    //void on_init() override;
    //void on_update() override;
    //
    //bool hit();
};
