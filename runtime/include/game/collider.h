#pragma once

#include "variant/variant_base.h"
#include "game/position.h"

class Collider : public VariantBase {
    VARIANT(Collider)

public:
    int m_collider_type = 0; PROPERTY() // 0=None, 1=Rectangle, 2=Circle
    bool m_is_trigger = false; PROPERTY()  
    
    float m_width = 0.0f; PROPERTY()
    float m_height = 0.0f; PROPERTY()
    float m_radius = 0.0f; PROPERTY()

    bool m_draw_debug = false; PROPERTY()
    
    void on_update() override;
    bool intersects(const Collider& other) const;

private:
    void debug_draw();
    Rectangle get_rectangle() const;
    Vector2 get_circle_center() const;
};
