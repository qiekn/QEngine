#pragma once

#include "variant/variant_base.h"

#include "game/position.h"
#include "game/velocity.h"
#include "game/collider.h"
#include "game/speed.h"

class Ball : public VariantBase {
    VARIANT(Ball);

public:
    void on_update() override;
    void on_play_start() override;
    void on_play_update() override;
    
    void launch();
    void reset_position(float x, float y);
    void handle_collision(Collider& other);
    
private:
    void handle_collisions();
    void handle_paddle_collision();
    void handle_brick_collision();
    void keep_in_bounds();

    bool m_launched = false;
};
