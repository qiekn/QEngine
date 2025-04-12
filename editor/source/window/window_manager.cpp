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

void WindowManager::render() {
    const auto& layout = LayoutConfig::get();
    
    float menu_bar_height = ImGui::GetFrameHeight();
    ImVec2 window_size = ImVec2(GetScreenWidth(), GetScreenHeight());
    
    float main_content_height = window_size.y - menu_bar_height - layout.console_height;

    ImVec2 hierarchy_pos = ImVec2(0, menu_bar_height);
    ImVec2 hierarchy_size = ImVec2(layout.hierarchy_width, main_content_height);
    
    ImVec2 asset_browser_pos = ImVec2(window_size.x - layout.asset_browser_width, menu_bar_height);
    ImVec2 asset_browser_size = ImVec2(layout.asset_browser_width, main_content_height);
    
    ImVec2 console_pos = ImVec2(0, menu_bar_height + main_content_height);
    ImVec2 console_size = ImVec2(window_size.x, layout.console_height);

    for (auto& window : m_windows) {
        if (!window.is_visible || !window.is_open)
            continue;
            
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
        
        // main windows
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

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            for (auto& window : m_windows) {
                if (window.name != "Hierarchy" && window.name != "Console" && window.name != "Asset Browser") {
                    if (ImGui::MenuItem(window.name.c_str(), nullptr, &window.is_visible)) {
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    for (auto& window : m_windows) {
        if (window.name != "Hierarchy" && window.name != "Console" && window.name != "Asset Browser" && 
            window.is_visible && window.is_open) {
            
            ImGuiWindowFlags flags = ImGuiWindowFlags_None;
            
            if (!window.position.x && !window.position.y) {
                ImGui::SetNextWindowPos(ImVec2(window_size.x * 0.5f, window_size.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            }
            
            if (window.size.x > 0 && window.size.y > 0) {
                ImGui::SetNextWindowSize(window.size, ImGuiCond_FirstUseEver);
            }
            
            if (ImGui::Begin(window.name.c_str(), &window.is_open)) {
                if (window.render_func) {
                    window.render_func(ImGui::GetWindowPos(), ImGui::GetWindowSize());
                }
                ImGui::End();
            }
            
            window.position = ImGui::GetWindowPos();
            window.size = ImGui::GetWindowSize();
        }
    }
}
