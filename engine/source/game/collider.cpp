#include "game/collider.h"
#include "game/position.h"

#include "core/query.h"
#include "raymath.h"

enum class ColliderType : int {
    None = 0,
    Rectangle = 1,
    Circle = 2,
};

void Collider::on_update() {
    debug_draw();
}

void Collider::on_play_update() {
    check_collisions();
}

void Collider::check_collisions() {
    if(m_static) { // if static, skip
        return;
    }

    if (m_is_trigger || m_collider_type == (int)ColliderType::None) {
        return;
    }

    auto& position = Query::read<Position>(this);

    Query::for_each<Collider>([this](Collider& other) {
        if(!m_enable || !other.m_enable) {
            return;
        }

        if (other.entity_id == entity_id) {
            return;
        }

        if (other.m_collider_type == (int)ColliderType::None) {
            return;
        }

        if (m_is_trigger || other.m_is_trigger) {
            return;
        }

        if (this->intersects(other)) {
            if(m_callback) {
                m_callback(other);
            }
        }
    });
}

bool Collider::intersects(const Collider& other) const {
    if (m_collider_type == (int)ColliderType::None || other.m_collider_type == (int)ColliderType::None) {
        return false;
    }

    if (m_collider_type == (int)ColliderType::Rectangle && other.m_collider_type == (int)ColliderType::Rectangle) {
        const auto& this_rect = get_rectangle();
        const auto& other_rect = other.get_rectangle();

        return CheckCollisionRecs(this_rect, other_rect);
    }

    if (m_collider_type == (int)ColliderType::Circle && other.m_collider_type == (int)ColliderType::Circle) {
        float distanceSq = Vector2DistanceSqr(get_circle_center(), other.get_circle_center());
        float combinedRadius = m_radius + other.m_radius;
        return distanceSq <= (combinedRadius * combinedRadius);
    }

    if ((m_collider_type == (int)ColliderType::Rectangle && other.m_collider_type == (int)ColliderType::Circle) ||
        (m_collider_type == (int)ColliderType::Circle && other.m_collider_type == (int)ColliderType::Rectangle)) {

        const Collider& rect_collider = (m_collider_type == (int)ColliderType::Rectangle) ? *this : other;
        const Collider& circle_collider = (m_collider_type == (int)ColliderType::Circle) ? *this : other;

        return CheckCollisionCircleRec(
            circle_collider.get_circle_center(), 
            circle_collider.m_radius, 
            rect_collider.get_rectangle()
        );
    }

    return false;
}

Rectangle Collider::get_rectangle() const {
    const auto& position = Query::get<Position>(this);

    return Rectangle{
        position.x - m_width / 2, 
        position.y - m_height / 2,
        m_width, 
        m_height
    };
}

Vector2 Collider::get_circle_center() const {
    const auto& position = Query::get<Position>(this);

    return Vector2{
        position.x,
        position.y
    };
}

void Collider::debug_draw() {
    if (!m_draw_debug) {
        return;
    }

    const auto& position = Query::get<Position>(this);
    Color color = m_is_trigger ? YELLOW : BLUE;

    switch (m_collider_type) {
        case (int)ColliderType::Rectangle:
            DrawRectangleLinesEx(get_rectangle(), 3, color);
            break;
        case (int)ColliderType::Circle:
            DrawCircleLines(
                position.x, 
                position.y, 
                m_radius, 
                color
            );
            break;
        default:
            break;
    }
}
