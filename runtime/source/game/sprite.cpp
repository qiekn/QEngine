#include "game/sprite.h"
#include "game/speed.h"

#include "raylib.h"

void Sprite::on_init() {
    if(!path_to_sprite.empty()) {
        this->texture = LoadTexture(path_to_sprite.c_str()); 
        m_texture_loaded = true;
    }
}

void Sprite::on_update() {
    if(!m_texture_loaded) return;

    auto& pos = get_variant_or_default<Position>(); 

    const float x = pos.x;
    const float y = pos.y;

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

void Sprite::on_path_to_sprite_set() {
    if(!path_to_sprite.empty()) {
        texture = LoadTexture(path_to_sprite.c_str());
        m_texture_loaded = true;
    }
}





