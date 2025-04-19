#pragma once

#include "variant/variant_base.h"
#include "game/brick.h"
#include "game/position.h"
#include "game/collider.h"

class BrickManager : public VariantBase {
    VARIANT(BrickManager);

public:
    int rows = 5; PROPERTY()
    int columns = 8; PROPERTY()

    float brick_width = 100.0f; PROPERTY()
    float brick_height = 30.0f; PROPERTY()

    float padding_x = 20.0f; PROPERTY()
    float padding_y = 20.0f; PROPERTY()

    float start_x = 400.0f; PROPERTY()
    float start_y = 100.0f; PROPERTY()
    
    //void on_init() override;
    //void create_bricks();
    
private:
    //void create_brick(float x, float y, int row, int col);
    //Color get_brick_color(int row) const;
};
