#include "game/sprite.h"

void Sprite::on_init() {
    this->texture = LoadTexture(path_to_sprite.c_str()); 
}

void Sprite::on_update() {
    rttr::variant pos_var;
    entity_get_variant<Position>(pos_var);

    if(!pos_var.is_valid()) { return; }

    const auto& pos = pos_var.get_value<Position>();
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
