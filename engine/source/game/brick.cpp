#include "game/brick.h"
#include "core/query.h"
#include "core/raylib_wrapper.h"

void Brick::on_play_update() {
    if(is_destroyed()) {
        return;
    }

    auto [position, collider] = Query::read<Position, Collider>(this);
    
    Rectangle rect = {
        position.x - collider.m_width / 2,
        position.y - collider.m_height / 2,
        collider.m_width,
        collider.m_height
    };
    
    draw_rectangle_rec(rect, m_color);
    draw_rectangle_lines_ex(rect, 2.0f, BLACK);
    
    char health_text[2];
    sprintf(health_text, "%d", m_health);
    draw_text(health_text, position.x - 5, position.y - 10, 20, WHITE);
}

void Brick::damage() {
    m_health -= 1;

    if(m_health == 0) {
        Query::get<Collider>(this).set_enable(false);
    }
}
