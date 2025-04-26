// window_manager.h
#pragma once

#include <functional>
#include <vector>
#include <string>
#include "imgui.h"

#include "raylib.h"

struct LayoutConfig {
    float hierarchy_width_ratio = 0.18;
    float console_height_ratio = 0.35;
    float asset_browser_width_ratio = 0.315;
    
    float hierarchy_width;
    float console_height;
    float asset_browser_width;
    
    LayoutConfig() {
        update_from_screen_size(GetScreenWidth(), GetScreenHeight());
    }
    
    void update_from_screen_size(float screen_width, float screen_height) {
        hierarchy_width = screen_width * hierarchy_width_ratio;
        console_height = screen_height * console_height_ratio;
        asset_browser_width = screen_width * asset_browser_width_ratio;
    }
    
    static LayoutConfig& get() {
        static LayoutConfig instance;
        return instance;
    }
};

struct WindowInfo {
    std::string name;
    ImVec2 position;
    ImVec2 size;
    bool is_open;
    bool is_visible;
    std::function<void(const ImVec2&, const ImVec2&)> render_func;
};

struct MenuInfo {
    std::string name;
    std::string category;
    std::function<void()> render_func;
    bool is_open = false;
};

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();
    
    void add_menu_item(const std::string& category, const std::string& name, std::function<void()> render_func);

    void add_window(const std::string& name, 
                  std::function<void(const ImVec2&, const ImVec2&)> render_func,
                  ImVec2 default_size = ImVec2(0, 0),
                  bool is_visible = true);

    bool is_window_visible(const std::string& name) const;

private:
    std::vector<WindowInfo> m_windows;
    std::vector<MenuInfo> m_menus;
};
