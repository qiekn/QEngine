#include "game/sprite.h"
#include "game/scale.h"

#include "raylib.h"
#include "core/query.h"

void Sprite::on_init() {
    if(!path_to_sprite.empty()) {
        this->texture = LoadTexture(path_to_sprite.c_str()); 
        m_texture_loaded = true;
    }
}

void Sprite::on_update() {
    if(!m_texture_loaded) return;

    const auto [position, scale] = Query::read<Position, Scale>(this);

    float width = texture.width * scale.x;
    float height = texture.height * scale.y;

    DrawTexturePro(
        texture,
        Rectangle{ 0, 0, (float)texture.width, (float)texture.height },  
        Rectangle{ position.x, position.y, width, height },  
        Vector2{ width/2, height/2 },  
        0.0f,  
        WHITE   
    );
}

void Sprite::handle_new_path() {
    if(!path_to_sprite.empty()) {
        texture = LoadTexture(path_to_sprite.c_str());
        m_texture_loaded = true;
    }

    log_info() << "Handle new path" << std::endl;
}





