#include "window/window_manager.h"
#include "raylib.h"

#include "engine/engine_event.h"

WindowManager::WindowManager() {}

void WindowManager::add_window(const std::string& name, 
                             std::function<void(const ImVec2&, const ImVec2&)> render_func,
                             ImVec2 default_size,
                             bool is_visible) {
    WindowInfo window;
    window.name = name;
    window.position = ImVec2(0, 0); 
    window.size = default_size;
    window.is_open = true;
    window.is_visible = is_visible;
    window.render_func = render_func;
    
    m_windows.push_back(window);
}

bool WindowManager::is_window_visible(const std::string& name) const {
    for (const auto& window : m_windows) {
        if (window.name == name) {
            return window.is_visible;
        }
    }
    return false;
}

void WindowManager::add_menu_item(const std::string& category, const std::string& name, std::function<void()> render_func) {
    MenuInfo info;
    info.category = category;
    info.name = name;
    info.render_func = render_func;
    m_menus.push_back(info);
}

void WindowManager::render() {
    const auto& layout = LayoutConfig::get();
    
    const float menu_bar_height = ImGui::GetFrameHeight();
    const ImVec2 window_size = ImVec2(GetScreenWidth(), GetScreenHeight());
    
    const float main_content_height = window_size.y - menu_bar_height - layout.console_height;

    const ImVec2 hierarchy_pos = ImVec2(0, menu_bar_height);
    const ImVec2 hierarchy_size = ImVec2(layout.hierarchy_width, main_content_height);
    
    const ImVec2 asset_browser_pos = ImVec2(window_size.x - layout.asset_browser_width, menu_bar_height);
    const ImVec2 asset_browser_size = ImVec2(layout.asset_browser_width, main_content_height);
    
    const ImVec2 console_pos = ImVec2(0, menu_bar_height + main_content_height);
    const ImVec2 console_size = ImVec2(window_size.x, layout.console_height);

    for (auto& window : m_windows) {
        if (!window.is_visible || !window.is_open)
            continue;
            
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
        
        if (window.name == "Hierarchy") {
            window.position = hierarchy_pos;
            window.size = hierarchy_size;
        }
        else if (window.name == "Asset Browser") {
            window.position = asset_browser_pos;
            window.size = asset_browser_size;
        }
        else if (window.name == "Console") {
            window.position = console_pos;
            window.size = console_size;
        }
        
        ImGui::SetNextWindowPos(window.position);
        ImGui::SetNextWindowSize(window.size);
        
        if (ImGui::Begin(window.name.c_str(), &window.is_open, flags)) {
            if (window.render_func) {
                window.render_func(window.position, window.size);
            }
            ImGui::End();
        }
    }

    // In window_manager.cpp - modify the render() method
if(ImGui::BeginMainMenuBar()) {
    for(auto& menu : m_menus) {
        if(menu.category.empty() && menu.name.empty()) {
            // This is a special case - render directly in main menu bar
            menu.render_func();
        }
        else if(menu.category.empty()) {
            // Regular menu items without category
            ImGui::MenuItem(menu.name.c_str(), nullptr, &menu.is_open);
        }
        else {
            // Dropdown menus
            if(ImGui::BeginMenu(menu.category.c_str())) {
                ImGui::MenuItem(menu.name.c_str(), nullptr, &menu.is_open);
                ImGui::EndMenu();
            }
        }
    }
    ImGui::EndMainMenuBar();
}

// Then process menu items that should open windows
for(auto& menu : m_menus) {
    if(!menu.category.empty() || !menu.name.empty()) { // Skip direct render components
        if(menu.is_open) {
            menu.render_func();
        }
    }
}

}
