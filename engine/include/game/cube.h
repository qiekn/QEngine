#pragma once
#include "variant/variant_base.h"
#include "game/position.h"

class Cube : public VariantBase {
    VARIANT(Cube);
public:
    float width = 50.0f; PROPERTY()
    float height = 50.0f; PROPERTY()
    Color color = RED; PROPERTY() // Nesting is also OK, Color is a Raylib struct.
    
    void on_init() override; // Lifetime method, called right after construction
    void on_update() override; // Lifetime method, called every frame
    void on_play_update() override; // Lifetime method, called every frame while in play mode
    /*  Not implemented ones:
        void on_post_init() override; // Called after on_init()
        void on_play_start() override; // Called on entering play mode
        void on_play_late_start() override // Called after on_play_start()
    */
    
private:
    void handle_input();
};
