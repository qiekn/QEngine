#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"

struct Sprite : VariantBase {
    Sprite() = default;
    Sprite(VariantCreateInfo info) : VariantBase(info) {}

    std::string path_to_sprite;
    Texture2D texture;
    bool isLoaded = false;

    void awake() override {
        //std::cout << "||||||||||||||||||||||||||||||||||||||||||" << std::endl;
        //texture = LoadTexture(path_to_sprite.c_str());
    }

    void tick() override { 
        if(!isLoaded) {
            texture = LoadTexture(path_to_sprite.c_str());
            isLoaded = true;
        }


        const auto& pos = entity_get_variant<Position>(); 
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

    RTTR_ENABLE(VariantBase);
};

