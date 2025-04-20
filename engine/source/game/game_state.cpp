#include "game/game.h"
#include "remote_logger/remote_logger.h"

void Game::start_game() {
    if (m_game_state == GameState::Idle) {
        m_game_state = GameState::Running;
        
        for (const auto& callback : m_game_start_callbacks) {
            if (callback) {
                callback();
            }
        }
        
        log_info() << "Game started" << std::endl;
    } else {
        log_warning() << "Cannot start game: game is not in Idle state" << std::endl;
    }
}

void Game::end_game() {
    if (m_game_state == GameState::Running) {
        m_game_state = GameState::End;
        
        for (const auto& callback : m_game_end_callbacks) {
            if (callback) {
                callback();
            }
        }
        
        log_info() << "Game ended" << std::endl;
    } else {
        log_warning() << "Cannot end game: game is not in Running state" << std::endl;
    }
}

void Game::restart_game() {
    if (m_game_state == GameState::End) {
        m_game_state = GameState::Idle;
        
        for (const auto& callback : m_game_restart_callbacks) {
            if (callback) {
                callback();
            }
        }
        
        log_info() << "Game restarted" << std::endl;
    } else {
        log_warning() << "Cannot restart game: game is not in End state" << std::endl;
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

void Game::register_on_game_restart(callback cb) {
    if (cb) {
        m_game_restart_callbacks.push_back(cb);
    }
}
