#pragma once

#include "variant/variant_base.h"
#include "game/collider.h"

class Brick : public VariantBase {
    VARIANT(Brick);

public:
    Brick(int health, Color color) : m_health(health), m_inital_health(health), m_color(color) {}

    void on_play_update() override;
    void damage();
    void reset();
    inline bool is_destroyed() { return m_health <= 0; }

    int get_initial_health() const { return m_inital_health; }

private:
    int m_health = 0;
    int m_inital_health = 0; // used for resetting
    Color m_color = RED; 
};
