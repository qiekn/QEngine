#pragma once

#include "variant/variant_base.h"
#include "game/collider.h"

class Brick : public VariantBase {
    VARIANT(Brick);

public:
    Brick(int health, Color color) : m_health(health), m_color(color) {}

    void on_play_update() override;

    void damage();
    inline bool is_destroyed() { return m_health <= 0; }

private:
    int m_health = 1; 
    Color m_color = RED; 
};
