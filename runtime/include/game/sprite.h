#pragma once

#include "raylib.h"
#include "core/variant/variant_base.h"

#include "game/position.h"
#include "game/speed.h"

class Sprite : public VariantBase { 
    VARIANT(Sprite);

public:
    std::string path_to_sprite; PROPERTY();

    void on_init() override;
    void on_post_init() override;
    void on_update() override;
    void on_play_update() override;

private:
    Texture texture; 
    void basic_move();

    VariantRef<Position> pos;
    VariantRef<Speed> speed;

    bool m_texture_loaded = false;
};

