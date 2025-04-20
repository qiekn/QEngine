#include "game/game.h"
#include "remote_logger/remote_logger.h"

#include "core/raylib_wrapper.h"

void Game::on_play_update() {
    if(is_key_pressed(KEY_SPACE)) {
        start_game();
    }
}

void Game::start_game() {
    if (m_game_state != GameState::Running) {
        m_game_state = GameState::Running;
        
        for (const auto& callback : m_game_start_callbacks) {
            if (callback) {
                callback();
            }
        }
        
        log_info() << "Game started" << std::endl;
    } else {
        log_warning() << "Cannot start game: Game is already running" << std::endl;
    }
}

void Game::end_game() {
    if (m_game_state != GameState::End) {
        m_game_state = GameState::End;
        
        for (const auto& callback : m_game_end_callbacks) {
            if (callback) {
                callback();
            }
        }
        
        log_info() << "Game ended" << std::endl;
    } else {
        log_warning() << "Cannot end game: Game has already ended" << std::endl;
    }
}

void Game::register_on_game_start(callback cb) {
    if (cb) {
        m_game_start_callbacks.push_back(cb);
    }
}

void Game::register_on_game_end(callback cb) {
    if (cb) {
        m_game_end_callbacks.push_back(cb);
    }
}
