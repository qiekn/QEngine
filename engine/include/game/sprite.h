#pragma once

#include "raylib.h"
#include "variant/variant_base.h"

#include "game/position.h"
#include "game/scale.h"

class Sprite : public VariantBase { 
    VARIANT(Sprite);

public:
    std::string path_to_sprite; PROPERTY() SET_CALLBACK(path_to_sprite);

    void on_init() override;
    void on_update() override;

private:
    void basic_move();

    Texture texture; 
    bool m_texture_loaded = false;
};

