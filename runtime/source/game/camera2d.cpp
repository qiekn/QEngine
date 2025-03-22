#include "game/camera2d.h"

void Camera2DSystem::on_init() {
    m_camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    m_camera.target = {0, 0};
    m_camera.rotation = 0.0f;
    m_camera.zoom = zoom;
}

void Camera2DSystem::on_update() {
    m_camera.zoom = zoom;
    
    if (enable_drag) {
        handle_dragging();
    }
    
    if (enable_zoom) {
        handle_zooming();
    }
}

void Camera2DSystem::handle_dragging() {
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mouse_position = GetMousePosition();
        
        if (!m_is_dragging) {
            m_is_dragging = true;
            m_previous_mouse_position = mouse_position;
        } else {
            Vector2 delta = {
                (m_previous_mouse_position.x - mouse_position.x) * drag_speed / zoom,
                (m_previous_mouse_position.y - mouse_position.y) * drag_speed / zoom
            };
            
            m_camera.target.x += delta.x;
            m_camera.target.y += delta.y;
            
            m_previous_mouse_position = mouse_position;
        }
    } else {
        m_is_dragging = false;
    }
}

void Camera2DSystem::handle_zooming() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
       
        Vector2 mouse_screen_pos = GetMousePosition();
        Vector2 mouse_world_pos_before = GetScreenToWorld2D(mouse_screen_pos, m_camera);
          
        float old_zoom = m_camera.zoom;
        float zoom_delta = wheel * zoom_increment * zoom;
        zoom += zoom_delta;
        

        if (zoom < min_zoom) zoom = min_zoom;
        if (zoom > max_zoom) zoom = max_zoom;
        
        m_camera.zoom = zoom;

        Vector2 mouse_world_pos_after = GetScreenToWorld2D(mouse_screen_pos, m_camera);

        m_camera.target.x += (mouse_world_pos_before.x - mouse_world_pos_after.x);
        m_camera.target.y += (mouse_world_pos_before.y - mouse_world_pos_after.y);
    }
}

Vector2 Camera2DSystem::screen_to_world(Vector2 screen_pos) const {
    return GetScreenToWorld2D(screen_pos, m_camera);
}

Vector2 Camera2DSystem::world_to_screen(Vector2 world_pos) const {
    return GetWorldToScreen2D(world_pos, m_camera);
}
