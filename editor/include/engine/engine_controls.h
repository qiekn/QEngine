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
    //bool m_start_pressed;

    void start_engine();
};
