#include "game/ball.h"

#include "core/query.h"
#include "core/raylib_wrapper.h"

#include "game/paddle.h"
#include "game/position.h"
#include "game/brick.h"
#include "game/tag.h"
#include "game/game.h"

void Ball::on_update() {
    auto& collider = Query::get<Collider>(this);
    draw_circle_v(collider.get_circle_center(), collider.get_radius(), GREEN);
}

void Ball::on_play_start() {
    auto& collider = Query::get<Collider>(this);
    collider.m_callback = [this](Collider& other) {
        handle_collision(other);
    };

    auto result = Query::find_first<Game>();
    if(result) {
        auto& game = result->get();
        game.register_on_game_start([this]() {
            launch();
        });

        game.register_on_game_end([this](){
                m_launched = false;
        });
    }
}

void Ball::on_play_update() {
    auto [position, velocity, collider] = Query::get<Position, Velocity, Collider>(this);

    if (!m_launched) {
        auto paddle_ref = Query::find_first<Paddle>();
        if (paddle_ref) {
            auto& paddle = paddle_ref->get();
            if(Query::has<Collider,Position>(paddle.get_id())) {
                auto [paddle_collider, paddle_position] = Query::get<Collider, Position>(paddle.get_id());
                float height = paddle_collider.m_height;
                position.x = paddle_position.x;
                position.y = paddle_position.y - collider.get_radius() - (height / 2);
            }
        }
        
        return;
    }
    
    float delta_time = get_frame_time();

    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;
}

void Ball::launch() {
    m_launched = true;
    
    auto [velocity, speed] = Query::get<Velocity, Speed>(this);

    velocity.x = get_random_value(-speed.value, speed.value);
    velocity.y = speed.value;
}

void Ball::reset_position(float x, float y) {
    auto& position = Query::get<Position>(this);
    position.x = x;
    position.y = y;
    m_launched = false;
}

void Ball::handle_collision(Collider& other) {
    auto [velocity, position, collider] = Query::get<Velocity, Position, Collider>(this);
    auto& other_position = Query::get<Position>(other.entity_id);

    if(other.m_collider_type == 1) { // Rectangle
        Rectangle rect = other.get_rectangle();

        float closest_x = fmaxf(rect.x, fminf(position.x, rect.x + rect.width));
        float closest_y = fmaxf(rect.y, fminf(position.y, rect.y + rect.height));

        Vector2 normal = {
            position.x - closest_x,
            position.y - closest_y
        };

        bool is_corner = (closest_x != position.x && closest_y != position.y);

        if (!is_corner && Vector2Length(normal) > 0) {
            if (fabsf(normal.x) < fabsf(normal.y)) {
                normal.x = 0; 
            } else {
                normal.y = 0;
            }
        }

        if (Vector2Length(normal) > 0) {
            normal = Vector2Normalize(normal);
        } else {
            normal = {0, -1};
        }

        float penetration = collider.m_radius - Vector2Distance({position.x, position.y}, {closest_x, closest_y});
        if (penetration > 0) {
            position.x += normal.x * penetration;
            position.y += normal.y * penetration;
        }

        float dot_product = velocity.x * normal.x + velocity.y * normal.y;
        if (dot_product < 0) {
            velocity.x = velocity.x - 2 * dot_product * normal.x;
            velocity.y = velocity.y - 2 * dot_product * normal.y;
        }
    }
    else if(other.m_collider_type == 2) { 
        Vector2 pos1 = {position.x, position.y};
        Vector2 pos2 = other.get_circle_center();

        Vector2 normal = Vector2Subtract(pos1, pos2);
        if (Vector2Length(normal) > 0) {
            normal = Vector2Normalize(normal);
        } else {
            normal = {0, -1}; 
        }

        float penetration = collider.m_radius + other.m_radius - Vector2Distance(pos1, pos2);
        if (penetration > 0) {
            position.x += normal.x * penetration;
            position.y += normal.y * penetration;
        }

        float dot_product = velocity.x * normal.x + velocity.y * normal.y;
        if (dot_product < 0) {
            velocity.x = velocity.x - 2 * dot_product * normal.x;
            velocity.y = velocity.y - 2 * dot_product * normal.y;
        }
    }

    if(Query::has<Brick>(other.entity_id)) {
        auto& brick = Query::get<Brick>(other.entity_id);
        brick.damage();
    }


    if(Query::has<Tag>(other.entity_id)) {
        const auto& tag = Query::read<Tag>(other.entity_id);
        if(tag.value == "bottom") {
            Query::find_first<Game>()->get().end_game();
        }
    }
}
