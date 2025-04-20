#include "game/score.h"
#include "core/raylib_wrapper.h"
#include "game/game.h"
#include "core/query.h"

void Score::on_play_start() {
    auto& game = Query::find_first<Game>();
    game.register_on_brick_destoryed([this](const auto& brick){
        on_break_destroyed(brick);
    });
}

void Score::add_points(int points) {
    value += points;
}

void Score::reset() {
    value = 0;
}

void Score::on_update() {
    char score_text[32];
    sprintf(score_text, "SCORE: %d", (int)value);
    draw_text(score_text, x, y, font_size, PURPLE);
}
