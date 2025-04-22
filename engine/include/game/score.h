#pragma once

#include "variant/variant_base.h"
#include "game/brick.h"

class Score : public VariantBase {
    VARIANT(Score);

public:
    float value = 0; PROPERTY();
    float point_base = 15; PROPERTY();
    float font_size = 30; PROPERTY();
    float x; PROPERTY();
    float y; PROPERTY();
    
    void on_play_start() override;
    void add_points(int points);
    void reset();
    
    inline void on_break_destroyed(const Brick& brick) { add_points(brick.get_initial_health() * point_base); }
    
    void on_update() override;
};
