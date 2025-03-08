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
    std::atomic<bool> m_compiled;
    std::atomic<bool> m_is_running;

    void start_engine();
};
