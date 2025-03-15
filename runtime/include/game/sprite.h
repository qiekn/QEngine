#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"


class Sprite : public VariantBase { 
    VARIANT(Sprite);

public:
    std::string path_to_sprite; PROPERTY();
    std::string extension; PROPERTY();

    void on_init() override;
    void on_update() override;
    void on_play_update() override;

private:
    Texture texture; 
    void basic_move();

    VariantRef<Position> position;
    
};

