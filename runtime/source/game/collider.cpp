#include "game/collider.h"

#include "core/query.h"
#include "raymath.h"

enum class ColliderType : int {
    None = 0,
    Rectangle = 1,
    Circle = 2,
};

void Collider::on_update() {
    debug_draw();

    Query::for_each<Collider>([this](Collider& collider) {
        if(collider.entity_id == entity_id) return; // this collider

        if(collider.intersects(*this)) {
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

    if ((m_collider_type == 1 && other.m_collider_type == 2) ||
        (m_collider_type == 2 && other.m_collider_type == 1)) {

        const Collider& rect_collider = (m_collider_type == 1) ? *this : other;
        const Collider& circle_collider = (m_collider_type == 2) ? *this : other;

        return CheckCollisionCircleRec(circle_collider.get_circle_center(), circle_collider.m_radius, rect_collider.get_rectangle());
    }

    return false;
}

Rectangle Collider::get_rectangle() const {
    auto position_ref = Query::get_component<Position>(this);

    if(!position_ref) {
        return {};
    }

    const auto& position = position_ref->get();

    Rectangle rect = {
        position.x - m_width / 2, 
        position.y - m_height / 2,
        m_width, 
        m_height
    };

    return rect;
}

Vector2 Collider::get_circle_center() const {
    auto position_ref = Query::get_component<Position>(this);

    if(!position_ref) {
        return {};
    }

    const auto& position = position_ref->get();

    return Vector2 {
        .x = position.x,
        .y = position.y,
    };
}

void Collider::debug_draw() {
    if (!m_draw_debug) {
        return;
    }

    auto position_ref = Query::get_component<Position>(this);

    if(!position_ref) {
        return;
    }

    const auto& position = position_ref->get();

    switch (m_collider_type) {
        case (int)ColliderType::Rectangle:  // rect
        {
            DrawRectangleLinesEx(get_rectangle(), 50, BLUE);
            break;
        }
        case (int)ColliderType::Circle: // circle
        {
            DrawCircleLines(
                position.x, 
                position.y, 
                m_radius, 
                RED
            );

            break;
        }
        default:
            break;
    }
}
