#pragma once

#include "variant/variant_base.h"
#include <vector>
#include <functional>

using callback = std::function<void()>;

enum class GameState {
    None = 0,
    Idle,
    Running,
    End,
    Length,
    // START: IDLE -> RUNNING, RESTART: END->IDLE, END: RUNNING->END
};

class Game : public VariantBase {
    VARIANT(Game);

public:
    GameState get_game_state() const { return m_game_state; }
    
    void start_game();
    void end_game();
    void restart_game();
    
    void register_on_game_start(callback cb);
    void register_on_game_end(callback cb);
    void register_on_game_restart(callback cb);

private:
    GameState m_game_state = GameState::Idle;
    std::vector<callback> m_game_start_callbacks;
    std::vector<callback> m_game_end_callbacks;
    std::vector<callback> m_game_restart_callbacks;
};
