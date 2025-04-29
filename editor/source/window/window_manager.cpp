#include "window/window_manager.h"
#include "raylib.h"
#include "engine/engine_event.h"
#include "imgui_internal.h"

WindowManager::WindowManager() {}

void WindowManager::render() {
    for (auto& window : m_windows) {
        if (!window.is_open)
            continue;
        
        if (ImGui::Begin(window.menu_info.name.c_str(), &window.is_open)) {
            if (window.render_func) {
                window.render_func();
            }
            ImGui::End();
        }
    }
}

void WindowManager::add_menu_item(const std::string& name, std::function<void()> render_func) {
    MenuInfo menu_info;
    menu_info.name = name;
    WindowInfo window_info;
    window_info.menu_info = menu_info;
    window_info.render_func = render_func;
    window_info.is_open = true;
    m_windows.push_back(window_info);
}
