#pragma once

#include <functional>
#include <vector>
#include <string>
#include "imgui.h"
#include "raylib.h"

struct MenuInfo {
    std::string name;
};

struct WindowInfo {
    bool is_open;
    std::function<void()> render_func;
    MenuInfo menu_info;
};

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();
    
    void add_menu_item(const std::string& name, std::function<void()> render_func);

private:
    std::vector<WindowInfo> m_windows;
};
