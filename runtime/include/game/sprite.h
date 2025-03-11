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

private:
    Texture texture; 
};

