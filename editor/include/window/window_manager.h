// window_manager.h
#pragma once

#include <functional>
#include <vector>
#include <string>
#include "imgui.h"

// TODO: Read this from .ini
struct LayoutConfig {
    float hierarchy_width = 460.0f;
    float console_height = 490.0f;
    float asset_browser_width = 810.0f;
    
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

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();
    
    void add_window(const std::string& name, 
                  std::function<void(const ImVec2&, const ImVec2&)> render_func,
                  ImVec2 default_size = ImVec2(0, 0),
                  bool is_visible = true);

    bool is_window_visible(const std::string& name) const;

private:
    std::vector<WindowInfo> m_windows;
};
