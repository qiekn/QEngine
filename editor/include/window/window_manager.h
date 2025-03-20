#pragma once

#include <functional>
#include "imgui.h"

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();

    inline void set_hierarchy_render_func(std::function<void()> func) { m_hierarchy_render_func = func; }
    inline void set_console_render_func(std::function<void(float, float, float)> func) { m_console_render_func = func; }
    inline void set_asset_browser_render_func(std::function<void()> func) { m_asset_browser_render_func = func; }

    float get_hierarchy_width() const { return m_hierarchy_width; }
    float get_console_height() const { return m_console_height; }

private:
    void sync_engine_window();

    float m_hierarchy_width = 460.0f;
    float m_console_height = 490.0f;
    float m_asset_browser_width = 810.0f;
    
    float m_sync_timer = 0.0f;
    const float m_sync_interval = 0.5f;

    std::function<void()> m_hierarchy_render_func;
    std::function<void(float, float, float)> m_console_render_func;
    std::function<void()> m_asset_browser_render_func;
};
