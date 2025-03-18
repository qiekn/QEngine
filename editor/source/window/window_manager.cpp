#include "window/window_manager.h"
#include "raylib.h"

WindowManager::WindowManager() 
    : m_hierarchy_render_func([]() {}),  
      m_content_render_func([]() {}),
      m_console_render_func([](float, float, float) {}) {
}

void WindowManager::draw_resize_handle(ImVec2 start, ImVec2 end, bool is_horizontal, bool is_hovered) {
    ImGui::GetForegroundDrawList()->AddRectFilled(
        start, end, 
        is_hovered ? IM_COL32(100, 100, 100, 255) : IM_COL32(80, 80, 80, 255)
    );
    
    const int num_lines = is_horizontal ? 3 : 5;
    const float line_length = is_horizontal ? (end.y - start.y) * 0.4f : (end.x - start.x) * 0.4f;
    const float spacing = is_horizontal ? (end.x - start.x) / (num_lines + 1) : (end.y - start.y) / (num_lines + 1);
    const ImVec2 center = ImVec2((start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f);
    
    ImU32 line_color = is_hovered ? IM_COL32(220, 220, 220, 200) : IM_COL32(180, 180, 180, 150);
    
    if (is_horizontal) {
        for (int i = 0; i < num_lines; i++) {
            float x = start.x + spacing * (i + 1);
            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(x, center.y - line_length/2),
                ImVec2(x, center.y + line_length/2),
                line_color, 1.0f
            );
        }
    } else {
        for (int i = 0; i < num_lines; i++) {
            float y = start.y + spacing * (i + 1);
            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(center.x - line_length/2, y),
                ImVec2(center.x + line_length/2, y),
                line_color, 1.0f
            );
        }
    }
}

void WindowManager::render() {
    float menu_bar_height = ImGui::GetFrameHeight();
    ImVec2 window_size = ImVec2(GetScreenWidth(), GetScreenHeight());
    
    float main_content_height = window_size.y - menu_bar_height - m_console_height;

    if (m_hierarchy_width < m_hierarchy_min_width) m_hierarchy_width = m_hierarchy_min_width;
    if (m_hierarchy_width > m_hierarchy_max_width) m_hierarchy_width = m_hierarchy_max_width;
    if (m_console_height < m_console_min_height) m_console_height = m_console_min_height;
    if (m_console_height > m_console_max_height) m_console_height = m_console_max_height;

    ImGui::SetNextWindowPos(ImVec2(0, menu_bar_height));
    ImGui::SetNextWindowSize(ImVec2(m_hierarchy_width, main_content_height));
    ImGuiWindowFlags hierarchy_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin("Hierarchy", nullptr, hierarchy_flags)) {
        m_hierarchy_render_func();
        ImGui::End();
    }

    const float resize_handle_width = 12.0f;
    ImVec2 resize_start(m_hierarchy_width - resize_handle_width/2, menu_bar_height);
    ImVec2 resize_end(m_hierarchy_width + resize_handle_width/2, menu_bar_height + main_content_height);
    
    bool is_hover_hierarchy_resize = ImGui::IsMouseHoveringRect(resize_start, resize_end);
    
    if (is_hover_hierarchy_resize) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        
        if (ImGui::IsMouseClicked(0)) {
            m_is_resizing_hierarchy = true;
        }
    }
    
    if (m_is_resizing_hierarchy) {
        m_hierarchy_width = ImGui::GetIO().MousePos.x;
        
        if (m_hierarchy_width < m_hierarchy_min_width) m_hierarchy_width = m_hierarchy_min_width;
        if (m_hierarchy_width > m_hierarchy_max_width) m_hierarchy_width = m_hierarchy_max_width;
        
        if (!ImGui::IsMouseDown(0)) {
            m_is_resizing_hierarchy = false;
        }
        
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    
    draw_resize_handle(resize_start, resize_end, false, is_hover_hierarchy_resize);

    ImGui::SetNextWindowPos(ImVec2(m_hierarchy_width + resize_handle_width/2, menu_bar_height));
    ImGui::SetNextWindowSize(ImVec2(window_size.x - m_hierarchy_width - resize_handle_width/2, main_content_height));
    ImGuiWindowFlags content_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | 
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    
    if (ImGui::Begin("Content", nullptr, content_flags)) {
        m_content_render_func();
        ImGui::End();
    }
    
    float console_y = menu_bar_height + main_content_height;
    const float console_resize_height = 16.0f;
    ImVec2 console_resize_start(0, console_y - console_resize_height/2);
    ImVec2 console_resize_end(window_size.x, console_y + console_resize_height/2);
    
    bool is_hover_console_resize = ImGui::IsMouseHoveringRect(console_resize_start, console_resize_end);
    
    if (is_hover_console_resize) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        
        if (ImGui::IsMouseClicked(0)) {
            m_is_resizing_console = true;
        }
    }
    
    if (m_is_resizing_console) {
        float mouse_y = ImGui::GetIO().MousePos.y;
        float new_console_height = window_size.y - mouse_y;
        
        if (new_console_height < m_console_min_height) new_console_height = m_console_min_height;
        if (new_console_height > m_console_max_height) new_console_height = m_console_max_height;
        
        m_console_height = new_console_height;
        main_content_height = window_size.y - menu_bar_height - m_console_height;
        console_y = menu_bar_height + main_content_height;
        
        if (!ImGui::IsMouseDown(0)) {
            m_is_resizing_console = false;
        }
        
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    
    draw_resize_handle(console_resize_start, console_resize_end, true, is_hover_console_resize);
    
    m_console_render_func(console_y, window_size.x, m_console_height);
}
