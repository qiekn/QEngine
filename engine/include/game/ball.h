#pragma once

#include "variant/variant_base.h"

#include "game/position.h"
#include "game/velocity.h"
#include "game/collider.h"
#include "game/speed.h"

class Ball : public VariantBase {
    VARIANT(Ball);

public:
    void on_init() override;
    void on_update() override;
    void on_play_update() override;

    float test; PROPERTY();
    
    void launch();
    void reset_position(float x, float y);
    
private:
    bool m_launched = false;
    void handle_collisions();
    void handle_paddle_collision();
    void handle_brick_collision();
    void keep_in_bounds();
};
