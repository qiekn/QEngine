#include "game/camera2d.h"

void Camera2DSystem::on_init() {
    auto& camera = get_zeytin().get_camera();
    camera.zoom = zoom;
    camera.target = m_target;
}

void Camera2DSystem::on_update() {
    auto& camera = get_zeytin().get_camera();
    
    camera.zoom = zoom;
    
    if (enable_drag) {
        handle_dragging();
    }
    
    if (enable_zoom) {
        handle_zooming();
    }

    m_target.x = camera.target.x;
    m_target.y = camera.target.y;
}

void Camera2DSystem::handle_dragging() {
    auto& camera = get_zeytin().get_camera();
    
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
            
            camera.target.x += delta.x;
            camera.target.y += delta.y;
            
            m_previous_mouse_position = mouse_position;
        }
    } else {
        m_is_dragging = false;
    }
}

void Camera2DSystem::handle_zooming() {
    auto& camera = get_zeytin().get_camera();
    
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouse_screen_pos = GetMousePosition();
        Vector2 mouse_world_pos_before = GetScreenToWorld2D(mouse_screen_pos, camera);
        
        float old_zoom = zoom;
        float zoom_delta = wheel * zoom_increment * zoom;
        zoom += zoom_delta;
        
        if (zoom < min_zoom) zoom = min_zoom;
        if (zoom > max_zoom) zoom = max_zoom;
        
        camera.zoom = zoom;
        
        Vector2 mouse_world_pos_after = GetScreenToWorld2D(mouse_screen_pos, camera);
        
        camera.target.x += (mouse_world_pos_before.x - mouse_world_pos_after.x);
        camera.target.y += (mouse_world_pos_before.y - mouse_world_pos_after.y);
    }
}

Vector2 Camera2DSystem::screen_to_world(Vector2 screen_pos) const {
    auto& camera = get_zeytin().get_camera();
    
    return GetScreenToWorld2D(screen_pos, camera);
}

Vector2 Camera2DSystem::world_to_screen(Vector2 world_pos) const {
    auto& camera = get_zeytin().get_camera();
    
    return GetWorldToScreen2D(world_pos, camera);
}
