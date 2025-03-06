#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"

struct Sprite : VariantBase {
    Sprite() = default;
    Sprite(VariantCreateInfo info) : VariantBase(info) {}

    std::string path_to_sprite;
    Texture2D texture;

    void awake() override {
        std::cout << "awake is called" << std::endl;
        std::cout << "Loading texture: " << path_to_sprite << std::endl;
        texture = LoadTexture(path_to_sprite.c_str());
    }

    void tick() override {
        std::cout << "tick is called" << std::endl;
        Position pos = entity_get_variant<Position>();
        DrawTexture(texture, 400, 300, WHITE);
    }

    RTTR_ENABLE(VariantBase);
};

