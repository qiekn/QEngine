#include "window/window_manager.h"
#include "raylib.h"
#include "logger.h"

WindowManager::WindowManager() 
    : m_hierarchy_render_func([]() {}),  
      m_content_render_func([]() {}),
      m_console_render_func([](float, float, float) {}),
      m_asset_browser_render_func([]() {}) {}

void WindowManager::render() {
    float menu_bar_height = ImGui::GetFrameHeight();
    ImVec2 window_size = ImVec2(GetScreenWidth(), GetScreenHeight());
    
    float main_content_height = window_size.y - menu_bar_height - m_console_height;

    ImVec2 hierarchy_pos = ImVec2(0, menu_bar_height);
    ImVec2 hierarchy_size = ImVec2(m_hierarchy_width, main_content_height);
    ImVec2 asset_browser_pos = ImVec2(window_size.x - m_asset_browser_width, menu_bar_height);
    ImVec2 asset_browser_size = ImVec2(m_asset_browser_width, main_content_height);

    ImGui::SetNextWindowPos(hierarchy_pos);
    ImGui::SetNextWindowSize(hierarchy_size);
    ImGuiWindowFlags hierarchy_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    

    if (ImGui::Begin("Hierarchy", nullptr, hierarchy_flags)) {
        ImGui::Separator();
        ImGui::Text("Hierarchy");

        if (ImGui::Button("+", ImVec2(20, 20))) {
            m_hierarchy_width += 10.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("-", ImVec2(20, 20))) {
            m_hierarchy_width -= 10.0f;
        }

        m_hierarchy_render_func();
        ImGui::End();
    }

    ImGui::SetNextWindowPos(asset_browser_pos);
    ImGui::SetNextWindowSize(asset_browser_size);
    ImGuiWindowFlags asset_browser_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin("Asset Browser", nullptr, asset_browser_flags)) {
        ImGui::Text("Asset Browser");
        ImGui::Separator();

        if (ImGui::Button("+", ImVec2(20, 20))) {
            m_asset_browser_width += 10.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("-", ImVec2(20, 20))) {
            m_asset_browser_width -= 10.0f;
        }

        m_asset_browser_render_func();
        ImGui::End();
    }

    ImGui::SetNextWindowPos(ImVec2(m_hierarchy_width, menu_bar_height));
    ImGui::SetNextWindowSize(ImVec2(window_size.x - m_hierarchy_width - m_asset_browser_width, main_content_height));
    ImGuiWindowFlags content_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | 
                                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    m_content_size = ImVec2(window_size.x - m_hierarchy_width - m_asset_browser_width, main_content_height);
    m_content_position = ImVec2(m_hierarchy_width + 5, m_content_size.y - 720 + 5);

    //log_info() << m_content_size.x << " | " << m_content_size.y << std::endl;
    //log_info() << m_content_position.x << " | " << m_content_position.y << std::endl;

    if(IsWindowMinimized()) {
        log_info() << IsWindowMinimized() << std::endl;
    }


    
    //if (ImGui::Begin("Content", nullptr, content_flags)) {
    //    m_content_render_func();
    //    ImGui::End();
    //}
    
    float console_y = menu_bar_height + main_content_height;
    const float console_resize_height = 16.0f;
    ImVec2 console_resize_start(0, console_y - console_resize_height / 2);
    ImVec2 console_resize_end(window_size.x, console_y + console_resize_height / 2);

    ImGui::SetNextWindowPos(ImVec2(0, console_y));
    ImGui::SetNextWindowSize(ImVec2(window_size.x, m_console_height));
    ImGuiWindowFlags console_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Console", nullptr, console_flags)) {
        ImGui::Text("Console Window");

        if (ImGui::Button("+", ImVec2(20, 20))) {
            m_console_height += 10.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("-", ImVec2(20, 20))) {
            m_console_height -= 10.0f;
        }

        m_console_render_func(console_y, window_size.x, m_console_height);
        ImGui::End();
    }
}

