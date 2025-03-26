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
    auto& collider = Query::acquire<Collider>(this);
    draw_circle_v(collider.get_circle_center(), collider.get_radius(), GREEN);
}

void Ball::on_play_update() {
    auto [position, velocity, collider] = Query::acquire_all<Position, Velocity, Collider>(this);

    if (!m_launched) {
        auto paddle_ref = Query::find_first<Paddle>();
        if (paddle_ref) {
            auto& paddle = paddle_ref->get();
            if(Query::has<Collider,Position>(paddle.get_id())) {
                auto [paddle_collider, paddle_position] = Query::acquire_all<Collider, Position>(paddle.get_id());
                float height = paddle_collider.m_height;
                position.x = paddle_position.x;
                position.y = paddle_position.y - collider.get_radius() - (height / 2);
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
    
    auto [velocity, speed] = Query::acquire_all<Velocity, Speed>(this);

    velocity.x = get_random_value(-speed.value, speed.value);
    velocity.y = -speed.value;
}

void Ball::reset_position(float x, float y) {
    auto& position = Query::acquire<Position>(this);
    position.x = x;
    position.y = y;
    m_launched = false;
}

void Ball::handle_collisions() {
    handle_paddle_collision();
    handle_brick_collision();
}

void Ball::handle_paddle_collision() {
    auto [position, velocity, speed, collider] = Query::acquire_all<Position, Velocity, Speed, Collider>(this);

    auto paddle_ref = Query::find_first<Paddle>();
    if (!paddle_ref) {
        log_warning() << "Paddle is not found by ball" << std::endl;
        return;
    }

    auto& paddle = paddle_ref->get();
    auto [paddle_pos, paddle_collider] = Query::acquire_all<Position, Collider>(paddle.get_id());

    if (check_collision_circle_rec(
            {position.x, position.y},
            collider.get_radius(),
            paddle_collider.get_rectangle()) && velocity.y > 0) {
        
        float hit_position = (position.x - (paddle_pos.x - paddle_collider.m_width/2)) / paddle_collider.m_width;
        float bounce_angle = (hit_position - 0.5f) * 60.0f; 
        
        float angle = bounce_angle * DEG2RAD;
        velocity.x = speed.value * sinf(angle);
        velocity.y = -speed.value * cosf(angle);
        
        position.y = paddle_pos.y - paddle_collider.m_height/2 - collider.get_radius() - 1.0f;
    }
}

void Ball::handle_brick_collision() {
}

void Ball::keep_in_bounds() {
    auto [position, velocity, collider] = Query::acquire_all<Position, Velocity, Collider>(this);
    float radius = collider.get_radius();
    
    if (position.x - radius < 0) {
        position.x = radius;
        velocity.x = -velocity.x;
    } else if (position.x + radius > VIRTUAL_WIDTH) {
        position.x = VIRTUAL_WIDTH - radius;
        velocity.x = -velocity.x;
    }
    
    if (position.y - radius < 0) {
        position.y = radius;
        velocity.y = -velocity.y;
    }
    
    if (position.y + radius > VIRTUAL_HEIGHT) {
        // reflect
    }
}
