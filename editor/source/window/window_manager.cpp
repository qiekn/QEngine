#include "window/window_manager.h"
#include "raylib.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "engine/engine_event.h"

WindowManager::WindowManager() 
    : m_hierarchy_render_func([]() {}),  
      m_console_render_func([](float, float, float) {}),
      m_asset_browser_render_func([]() {}) {


    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EngineStarted,
        [this](bool) {
                sync_engine_window();
            }
    );

}

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

        m_hierarchy_render_func();
        ImGui::End();
    }

    ImGui::SetNextWindowPos(asset_browser_pos);
    ImGui::SetNextWindowSize(asset_browser_size);
    ImGuiWindowFlags asset_browser_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin("Asset Browser", nullptr, asset_browser_flags)) {
        m_asset_browser_render_func();
        ImGui::End();
    }

    m_sync_timer += ImGui::GetIO().DeltaTime;
    
    if (m_sync_timer >= m_sync_interval) {
        sync_engine_window();
        m_sync_timer = 0.0f;
    }

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

        m_console_render_func(console_y, window_size.x, m_console_height);
        ImGui::End();
    }
}


void WindowManager::sync_engine_window() {
    rapidjson::Document document;
    document.SetObject();
    auto& allocator = document.GetAllocator();


    document.AddMember("type", "window_state", allocator);
    document.AddMember("is_minimize", IsWindowMinimized(), allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    
    EngineEventBus::get().publish<const std::string&>(EngineEvent::WindowStateChanged, buffer.GetString());
}

