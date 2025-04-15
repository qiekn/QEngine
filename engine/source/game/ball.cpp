#include "game/ball.h"

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
    auto [position, velocity, collider] = Query::acquire<Position, Velocity, Collider>(this);

    if (!m_launched) {
        auto paddle_ref = Query::find_first<Paddle>();
        if (paddle_ref) {
            auto& paddle = paddle_ref->get();
            if(Query::has<Collider,Position>(paddle.get_id())) {
                auto [paddle_collider, paddle_position] = Query::acquire<Collider, Position>(paddle.get_id());
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
}

void Ball::launch() {
    m_launched = true;
    
    auto [velocity, speed] = Query::acquire<Velocity, Speed>(this);

    velocity.x = get_random_value(-speed.value, speed.value);
    velocity.y = speed.value;
}

void Ball::reset_position(float x, float y) {
    auto& position = Query::acquire<Position>(this);
    position.x = x;
    position.y = y;
    m_launched = false;
}
