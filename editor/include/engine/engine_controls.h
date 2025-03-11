#pragma once

#include <atomic>
#include <thread>
#include <imgui.h>

class EngineControls {
public:
    EngineControls();
    ~EngineControls() = default;

    void render_main_menu_controls();

private:
    std::atomic<bool> m_is_running;
    bool m_is_play_mode ;
    bool m_is_paused;

    void start_engine();

    void enter_play_mode();
    void pause_play_mode();
    void unpause_play_mode();
    void exit_play_mode();
};
