#include "game/brick_manager.h"
#include "core/query.h"
#include "game/game.h"

void BrickManager::on_play_start() {
    create_bricks();

    auto result = Query::find_first<Game>();
    if(result) {
        auto& game = result->get();
        game.register_on_game_end([this] {
            Query::for_each<Brick>([this](Brick& brick){
                // rebuild it, just the way it was, brick by brick
                brick.reset();
            });
        });
    }
}

void BrickManager::create_bricks() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            float x = start_x + col * (brick_width + padding_x);
            float y = start_y + row * (brick_height + padding_y);
            
            create_brick(x, y, row, col);
        }
    }
}

void BrickManager::create_brick(float x, float y, int row, int col) {
    auto entity = Query::create_entity();
    
    Query::add<Position>(entity, x, y);
    auto& brick = Query::add<Brick>(entity, row % 3 + 1, get_brick_color(row))->get();

    auto& collider = Query::add<Collider>(entity)->get();
    collider.m_collider_type = 1; 
    collider.m_width = brick_width;
    collider.m_height = brick_height;
}

Color BrickManager::get_brick_color(int row) const {
    switch (row % 5) {
        case 0: return RED;
        case 1: return GREEN;
        case 2: return BLUE;
        case 3: return GRAY;
        case 4: return PURPLE;
        default: return WHITE;
    }
}
