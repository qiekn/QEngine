#pragma once

class EngineControls {
public:
    EngineControls();
    ~EngineControls();

    void render_main_menu_controls();
    void start_engine();
    void kill_engine();

private:
    void render_file_menu();
    void render_edit_menu();
    void render_engine_controls();
    void render_play_controls();
    
    void enter_play_mode();
    void exit_play_mode();

    bool m_is_running;
    bool m_is_play_mode;
    bool m_is_paused;
    bool m_is_engine_starting;
};
