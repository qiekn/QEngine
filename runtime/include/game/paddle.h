#pragma once

#include "variant/variant_base.h"
#include "game/position.h"

class Paddle : public VariantBase {
    VARIANT(Paddle);
    REQUIRES(Position);

public:
    float width = 100.0f; PROPERTY()
    float height = 20.0f; PROPERTY()
    float speed = 400.0f; PROPERTY()
    
    void on_init() override;
    void on_update() override;
    void on_play_update() override;
    
private:
    void handle_input();
};
