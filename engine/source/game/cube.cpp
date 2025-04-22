#include "game/cube.h"
#include "game/speed.h"
#include "core/query.h"
#include "core/raylib_wrapper.h"

void Cube::on_init() {
    // Initialization if needed
    log_info() << "Cube is initialized" << std::endl;
}

void Cube::on_update() {
    // Get the position component
    auto& position = Query::get<Position>(this);
    
    // Draw the cube
    draw_rectangle(
        position.x - width / 2,
        position.y - height / 2,
        width,
        height,
        color);
}

void Cube::on_play_update() {
    // Handle user input during play mode
    handle_input();
}

void Cube::handle_input() {
    auto& position = Query::get<Position>(this);
    // Using Query::read for Speed as it's read-only
    const auto& speed = Query::read<Speed>(this);
    
    float delta_time = get_frame_time();
    float movement_speed = speed.value;
    
    // Handle keyboard input for movement
    if (is_key_down(KEY_LEFT) || is_key_down(KEY_A)) {
        position.x -= movement_speed * delta_time;
    }

    if (is_key_down(KEY_RIGHT) || is_key_down(KEY_D)) {
        position.x += movement_speed * delta_time;
    }

    if (is_key_down(KEY_UP) || is_key_down(KEY_W)) {
        position.y -= movement_speed * delta_time;
    }

    if (is_key_down(KEY_DOWN) || is_key_down(KEY_S)) {
        position.y += movement_speed * delta_time;
    }
}
