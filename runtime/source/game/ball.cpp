#include "game/ball.h"

#include <cmath>

#include "game/paddle.h"
#include "game/position.h"

#include "core/query.h"
#include "core/raylib_wrapper.h"

void Ball::on_init() {
    m_launched = false;
}

void Ball::on_update() {
    auto collider_ref = Query::get_component<Collider>(this);
    if(collider_ref) {
        const auto& circular_collider = collider_ref->get();
        draw_circle_v(circular_collider.get_circle_center(), circular_collider.get_radius(), GREEN);
    }
}

void Ball::on_play_update() {
    auto components = Query::get_components<Position, Velocity, Collider>(this);
    if (!components) return;
    
    auto& [position_ref, velocity_ref, collider_ref] = *components;
    auto& position = position_ref.get();
    auto& velocity = velocity_ref.get();
    auto& collider = collider_ref.get();
    
    if (!m_launched) {
        auto paddle = Query::find_first<Paddle>();
        if (paddle) {
            auto paddle_collider = Query::get_component<Collider>(paddle->get().entity_id);
            auto paddle_position = Query::get_component<Position>(paddle->get().entity_id);
            if (paddle_collider && paddle_position) {
                float height = paddle_collider->get().m_height;
                position.x = paddle_position->get().x;
                position.y = paddle_position->get().y - collider.get_radius() - (height / 2);
            }
        }
        
        if (is_key_pressed(KEY_SPACE)) {
            launch();
        }
        return;
    }
    
    float delta_time = get_frame_time();
    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;
    
    handle_collisions();
    keep_in_bounds();
}

void Ball::launch() {
    m_launched = true;
    
    auto components = Query::get_components<Velocity, Speed>(this);

    if(components) {
        auto& [velocity_ref, speed_ref] = *components;
        velocity_ref.get().x = get_random_value(-speed_ref.get().value, speed_ref.get().value);
        velocity_ref.get().y = -speed_ref.get().value;
        log_info() << "Velocity is set to : (" << velocity_ref.get().x << "," << velocity_ref.get().y << ")" << std::endl;
    }
}

void Ball::reset_position(float x, float y) {
    //auto position_ref = Query::get_component<Position>(this);
    //if (!position_ref) return;
    //
    //auto& position = position_ref->get();
    //position.x = x;
    //position.y = y;
    //
    //m_launched = false;
}

void Ball::handle_collisions() {
    //handle_paddle_collision();
    //handle_brick_collision();
}

void Ball::handle_paddle_collision() {
    //auto components = Query::get_components<Position, Velocity>(this);
    //if (!components) return;
    //
    //auto& [position_ref, velocity_ref] = *components;
    //auto& position = position_ref.get();
    //auto& velocity = velocity_ref.get();
    //
    //// Find the paddle
    //auto paddle = Query::find_first<Paddle>();
    //if (!paddle) return;
    //
    //auto paddle_pos = Query::get_component<Position>(&paddle.value());
    //if (!paddle_pos) return;
    //
    //float paddle_width = paddle->width;
    //float paddle_height = paddle->height;
    //float paddle_x = paddle_pos->get().x;
    //float paddle_y = paddle_pos->get().y;
    //
    //// Calculate paddle bounds
    //Rectangle paddleRect = {
    //    paddle_x - paddle_width/2,
    //    paddle_y - paddle_height/2,
    //    paddle_width,
    //    paddle_height
    //};
    //
    //// Check for collision with paddle
    //if (CheckCollisionCircleRec(
    //        {position.x, position.y},
    //        radius,
    //        paddleRect) && velocity.y > 0) {
    //    
    //    // Bounce based on where the ball hit the paddle
    //    float hitPosition = (position.x - (paddle_x - paddle_width/2)) / paddle_width;
    //    float bounceAngle = (hitPosition - 0.5f) * 60.0f; // -30 to +30 degrees
    //    
    //    // Convert angle to velocity
    //    float angle = bounceAngle * DEG2RAD;
    //    velocity.x = speed * sinf(angle);
    //    velocity.y = -speed * cosf(angle);
    //    
    //    // Ensure ball doesn't get stuck in paddle
    //    position.y = paddle_y - paddle_height/2 - radius - 1.0f;
    //}
}

void Ball::handle_brick_collision() {
    // This will be implemented later when we add the Brick variant
}

void Ball::keep_in_bounds() {
    //auto components = Query::get_components<Position, Velocity>(this);
    //if (!components) return;
    //
    //auto& [position_ref, velocity_ref] = *components;
    //auto& position = position_ref.get();
    //auto& velocity = velocity_ref.get();
    //
    //// Get screen dimensions
    //int screen_width = GetScreenWidth();
    //int screen_height = GetScreenHeight();
    //
    //// Handle horizontal bounds
    //if (position.x - radius < 0) {
    //    position.x = radius;
    //    velocity.x = -velocity.x;
    //} else if (position.x + radius > screen_width) {
    //    position.x = screen_width - radius;
    //    velocity.x = -velocity.x;
    //}
    //
    //// Handle top bound
    //if (position.y - radius < 0) {
    //    position.y = radius;
    //    velocity.y = -velocity.y;
    //}
    //
    //// Handle bottom bound (game over condition)
    //if (position.y + radius > screen_height) {
    //    // Reset ball to starting position
    //    reset_position(screen_width / 2, screen_height / 2);
    //    
    //    // ToDo: Handle lives decrement here
    //}
}
