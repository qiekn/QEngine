#include "game/collider.h"
#include "game/position.h"
#include "game/velocity.h"

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


    if(!Query::has<Position>(this)) {
        return;
    }

    auto position = Query::get<Position>(this);

    Query::for_each<Collider>([this](Collider& other) {
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
            handle_collision(other);
        }
    });
}

void Collider::handle_collision(Collider& other) {
    if(!Query::has<Velocity>(this)) { 
        return;
    }

    auto& velocity = Query::get<Velocity>(this);

    if(!Query::has<Position>(this) || !Query::has<Position>(other.entity_id)) {
        return;
    }

    auto& position = Query::get<Position>(this);
    auto& other_position = Query::get<Position>(other.entity_id);

    Vector2 normal;
    
    if (m_collider_type == (int)ColliderType::Circle && other.m_collider_type == (int)ColliderType::Circle) {
        normal = vector2_subtract(
            {position.x, position.y},
            {other_position.x, other_position.y}
        );
        normal = Vector2Normalize(normal);
    }
    else if (m_collider_type == (int)ColliderType::Rectangle && other.m_collider_type == (int)ColliderType::Rectangle) {
        Rectangle this_rect = get_rectangle();
        Rectangle other_rect = other.get_rectangle();
        
        float overlap_x = 0;
        float overlap_y = 0;
        
        float this_half_width = this_rect.width / 2;
        float other_half_width = other_rect.width / 2;
        float this_center_x = this_rect.x + this_half_width;
        float other_center_x = other_rect.x + other_half_width;
        float delta_x = this_center_x - other_center_x;
        float min_half_width = this_half_width + other_half_width;
        
        overlap_x = (delta_x > 0) ? 
                    (min_half_width - (this_center_x - other_half_width - other_rect.x)) :
                    (min_half_width - (other_center_x - this_half_width - this_rect.x));
        
        float this_half_height = this_rect.height / 2;
        float other_half_height = other_rect.height / 2;
        float this_center_y = this_rect.y + this_half_height;
        float other_center_y = other_rect.y + other_half_height;
        float delta_y = this_center_y - other_center_y;
        float min_half_height = this_half_height + other_half_height;
        
        overlap_y = (delta_y > 0) ?
                    (min_half_height - (this_center_y - other_half_height - other_rect.y)) :
                    (min_half_height - (other_center_y - this_half_height - this_rect.y));
        
        if (overlap_x < overlap_y) {
            normal = {(delta_x > 0) ? 1.0f : -1.0f, 0};
            
            position.x += (delta_x > 0) ? overlap_x : -overlap_x;
        } else {
            normal = {0, (delta_y > 0) ? 1.0f : -1.0f};
            
            position.y += (delta_y > 0) ? overlap_y : -overlap_y;
        }
    }
    else if (m_collider_type == (int)ColliderType::Circle && other.m_collider_type == (int)ColliderType::Rectangle) {
        Rectangle rect = other.get_rectangle();
        
        float closest_x = fmaxf(rect.x, fminf(position.x, rect.x + rect.width));
        float closest_y = fmaxf(rect.y, fminf(position.y, rect.y + rect.height));
        
        normal = vector2_subtract(
            {position.x, position.y},
            {closest_x, closest_y}
        );
        normal = Vector2Normalize(normal);
        
        float penetration = m_radius - Vector2Distance({position.x, position.y}, {closest_x, closest_y});
        position.x += normal.x * penetration;
        position.y += normal.y * penetration;
    }
    else if (m_collider_type == (int)ColliderType::Rectangle && other.m_collider_type == (int)ColliderType::Circle) {
        Rectangle rect = get_rectangle();
        
        float closest_x = fmaxf(rect.x, fminf(other_position.x, rect.x + rect.width));
        float closest_y = fmaxf(rect.y, fminf(other_position.y, rect.y + rect.height));
        
        normal = Vector2Subtract(
            {closest_x, closest_y},
            {other_position.x, other_position.y}
        );
        normal = Vector2Normalize(normal);
    }

    float dot_product = velocity.x * normal.x + velocity.y * normal.y;
    
    if (dot_product < 0) {
        velocity.x = velocity.x - 2 * dot_product * normal.x;
        velocity.y = velocity.y - 2 * dot_product * normal.y;
    }
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
