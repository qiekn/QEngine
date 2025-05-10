#include "game/paddle.h"
#include "game/position.h"
#include "core/query.h"
#include "core/raylib_wrapper.h"

void Paddle::on_init() {}

void Paddle::on_update() {
    auto& position = Query::get<Position>(this);
    
    draw_rectangle(
        position.x - width / 2,
        position.y - height / 2,
        width,
        height,
        BLUE);
}

void Paddle::on_play_update() {
    handle_input();
}

void Paddle::handle_input() {
    auto& position = Query::get<Position>(this);
    
    float delta_time = GetFrameTime();
    
    if (is_key_down(KEY_LEFT) || is_key_down(KEY_A)) {
        position.x -= speed * delta_time;
    }

    if (is_key_down(KEY_RIGHT) || is_key_down(KEY_D)) {
        position.x += speed * delta_time;
    }
}
