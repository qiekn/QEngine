#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"


class Sprite : public VariantBase {

public:
    Sprite() = default;
    Sprite(VariantCreateInfo info) : VariantBase(info) {}

    std::string path_to_sprite; PROPERTY();
 

    void awake() override {
        this->texture = LoadTexture(path_to_sprite.c_str()); 
    }

    void tick() override { 

        rttr::variant pos_var;
        entity_get_variant<Position>(pos_var);

        if(!pos_var.is_valid()) {
            std::cerr << "Position component is not found where sprite is attached" << std::endl;
            return;
        }

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

private:
    Texture texture; 

    RTTR_ENABLE(VariantBase);
};

