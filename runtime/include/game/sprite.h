#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"


class Sprite : public VariantBase { 
    VARIANT(Sprite);

public:
    std::string path_to_sprite; PROPERTY();

    void on_init() override;
    void on_update() override;
    void on_play_update() override;

private:
    void basic_move();

    VariantRef<Position> position;
    
    Texture texture; 
};

