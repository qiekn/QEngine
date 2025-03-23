#include "game/player.h"
#include "game/position.h"
#include "game/speed.h"

#include "raylib.h"
#include "core/query.h"

void Player::on_play_update() {
    move();
}

void Player::move() {
    auto components = Query::get_components<Position, Speed>(this);
    if (components) {
        auto& [pos, spd] = *components;
        auto& position = pos.get();
        auto& speed = spd.get();


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
}




