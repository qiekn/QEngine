#pragma once

#include "core/variant/variant_base.h"

class Player : public VariantBase {
    VARIANT(Player);

    void on_play_update() override;

private:
    void move();
};
