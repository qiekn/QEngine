#pragma once

#include "variant/variant_base.h"
#include "raylib.h"

class Camera2DSystem : public VariantBase {
    VARIANT(Camera2DSystem);

public:
    float zoom = 1.0f; PROPERTY()
    bool enable_drag = true; PROPERTY()
    bool enable_zoom = true; PROPERTY()
    float zoom_increment = 0.1f; PROPERTY()
    float min_zoom = 0.1f; PROPERTY()
    float max_zoom = 10.0f; PROPERTY()
    float drag_speed = 1.0f; PROPERTY()

    Vector2 m_target; PROPERTY() DEBUG()

    void on_init() override;
    void on_update() override;
    
    Vector2 screen_to_world(Vector2 screen_pos) const;
    Vector2 world_to_screen(Vector2 world_pos) const;

private:
    Vector2 m_drag_start = {0};
    Vector2 m_previous_mouse_position = {0};
    bool m_is_dragging = false;
    
    void handle_dragging();
    void handle_zooming();
};
