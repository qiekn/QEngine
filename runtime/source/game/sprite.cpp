#include "game/sprite.h"
#include "game/speed.h"

void Sprite::on_init() {

    if(path_to_sprite.empty()) {
        std::cout << "Texture is not loaded" << std::endl;
        return;
    }

    this->texture = LoadTexture(path_to_sprite.c_str()); 
    m_texture_loaded = true;
}

void Sprite::on_post_init() {
    pos = get_variant<Position>();
    speed = get_variant<Speed>();
}

void Sprite::on_update() {
    if(!m_texture_loaded) return;

    const float x = pos->get().x;
    const float y = pos->get().y;

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
    if(!pos.has_value() || !speed.has_value()) return;

    const float value = speed->get().value;

    if (IsKeyDown(KEY_RIGHT)) {
        pos->get().x += value;
    }
    if (IsKeyDown(KEY_LEFT)) {
        pos->get().x -= value;
    }
    if (IsKeyDown(KEY_DOWN)) {
        pos->get().y += value;
    }
    if (IsKeyDown(KEY_UP)) {
        pos->get().y -= value;
    }
}
