#include "game/sprite.h"
#include "game/speed.h"

void Sprite::on_init() {
    position = get_variant<Position>();
    this->texture = LoadTexture(path_to_sprite.c_str()); 
}

void Sprite::on_update() {
    const float x = position->get().x;
    const float y = position->get().y;

    float scale = 0.25;
    float width = texture.width * scale;
    float height = texture.height * scale;

    DrawTexturePro(
        texture,
        Rectangle{ 0, 0, (float)texture.width, (float)texture.height },  
        Rectangle{ x, y, width, height },  
        Vector2{ width/2, height/2 },  
        0.0f,  
        WHITE   
    );
}

void Sprite::on_play_update() {
    auto speed = get_variant<Speed>();
    const float value = speed->get().value;

    if (IsKeyDown(KEY_RIGHT)) {
        position->get().x += value;
    }
    if (IsKeyDown(KEY_LEFT)) {
        position->get().x -= value;
    }
    if (IsKeyDown(KEY_DOWN)) {
        position->get().y += value;
    }
    if (IsKeyDown(KEY_UP)) {
        position->get().y -= value;
    }
}
