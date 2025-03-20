#include "game/player.h"
#include "game/position.h"
#include "game/speed.h"

#include "raylib.h"

void Player::on_play_update() {
    move();
}

void Player::move() {
    auto pos_ref = get_variant<Position>();
    if(!pos_ref.has_value()) return;
    auto& position = pos_ref->get();

    auto speed_ref = get_variant<Speed>();
    if(!speed_ref.has_value()) return;
    auto& speed = speed_ref->get();

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        position.y -= speed.value;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        position.y += speed.value;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        position.x -= speed.value;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        position.x += speed.value;
    }
}




