#pragma once

#include <functional>
#include "imgui.h"

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();

    void set_hierarchy_render_func(std::function<void()> func) { m_hierarchy_render_func = func; }
    void set_content_render_func(std::function<void()> func) { m_content_render_func = func; }
    void set_console_render_func(std::function<void(float, float, float)> func) { m_console_render_func = func; }

    float get_hierarchy_width() const { return m_hierarchy_width; }
    float get_console_height() const { return m_console_height; }

private:
    void draw_resize_handle(ImVec2 start, ImVec2 end, bool is_horizontal, bool is_hovered);

    float m_hierarchy_width = 300.0f;
    float m_hierarchy_min_width = 200.0f;
    float m_hierarchy_max_width = 500.0f;
    bool m_is_resizing_hierarchy = false;

    float m_console_height = 300.0f;
    float m_console_min_height = 300.0f;
    float m_console_max_height = 500.0f;
    bool m_is_resizing_console = false;

    std::function<void()> m_hierarchy_render_func;
    std::function<void()> m_content_render_func;
    std::function<void(float, float, float)> m_console_render_func;
};
