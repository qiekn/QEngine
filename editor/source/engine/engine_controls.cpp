#include "engine/engine_controls.h"
#include <iostream>
#include <cstdlib>
#include <thread>

#include "imgui.h"
#include "engine/engine_event.h"

EngineControls::EngineControls() 
    : m_is_running(false)
    , m_is_play_mode(false)
    , m_is_paused(false)
    , m_is_engine_starting(false)
{
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EngineStarted,
        [this](const bool& success) {
            if (success) {
                m_is_running = true;
                m_is_engine_starting = false;
            }
        }
    );
}

void EngineControls::render_main_menu_controls() {
    if (ImGui::BeginMainMenuBar()) {
        render_file_menu();
        render_edit_menu();
        
        ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
        render_engine_controls();
        
        ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
        render_play_controls();
        
        ImGui::EndMainMenuBar();
    }
}

void EngineControls::render_file_menu() {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Exit", "Alt+F4")) {}
        ImGui::EndMenu();
    }
}

void EngineControls::render_edit_menu() {
    if (ImGui::BeginMenu("Edit")) {
        ImGui::EndMenu();
    }
}

void EngineControls::render_engine_controls() {
    if (ImGui::Button("Kill Engine")) {
        kill_engine();
    }

    if (!m_is_running) {
        ImGui::SameLine();
        if (!m_is_engine_starting) {
            if (ImGui::Button("Start Engine")) {
                m_is_engine_starting = true;
                start_engine();
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGui::Button("Starting...");
            ImGui::PopStyleColor(4);
        }
    } else {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Engine Running");
    }
}

void EngineControls::render_play_controls() {
    if (m_is_running) {
        // Play button
        ImGui::PushStyleColor(ImGuiCol_Button, m_is_play_mode ? ImVec4(0.0f, 0.5f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_is_play_mode ? ImVec4(0.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_is_play_mode ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        
        if (ImGui::Button("Play")) {
            m_is_play_mode = !m_is_play_mode;
            m_is_play_mode ? enter_play_mode() : exit_play_mode();
        }
        
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, m_is_paused ? ImVec4(0.8f, 0.5f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_is_paused ? ImVec4(0.9f, 0.6f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_is_paused ? ImVec4(1.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        
        if (ImGui::Button("Pause")) {
            m_is_paused = !m_is_paused;
            if (m_is_paused) {
                EngineEventBus::get().publish<bool>(EngineEvent::PausePlayMode, true);
            } else {
                EngineEventBus::get().publish<bool>(EngineEvent::UnPausePlayMode, true);
            }
        }
        
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        if (m_is_play_mode) {
            if (m_is_paused) {
                ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "PAUSED");
            } else {
                ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "PLAYING");
            }
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        
        ImGui::Button("Play");
        ImGui::SameLine();
        ImGui::Button("Pause");
        
        ImGui::PopStyleColor(4);
    }
}

void EngineControls::start_engine() {
    std::thread([this]() {
        #ifdef _WIN32
        int result = std::system("cd ../runtime && build.sh && run.sh");
        #else
        int result = std::system("cd ../runtime && ./build.sh && ./run.sh");
        #endif

        std::this_thread::sleep_for(std::chrono::seconds(20));
        if (!m_is_running) {
            m_is_engine_starting = false;
            std::cout << "Engine failed to start or timed out" << std::endl;
        }

        if (m_is_running) {
            std::cout << "Engine process exited with code " << result << std::endl;
            m_is_running = false;
            m_is_engine_starting = false;
        }
    }).detach();
}



void EngineControls::kill_engine() {
    m_is_play_mode = false;
    m_is_running = false;
    m_is_paused = false;
    m_is_engine_starting = false;

    std::thread([this]() {
        #ifdef _WIN32
        int result = std::system("kill_all_engines.sh");
        #else
        int result = std::system("./kill_all_engines.sh");
        #endif
    }).detach();
}

void EngineControls::enter_play_mode() {
    EngineEventBus::get().publish<bool>(EngineEvent::EnterPlayMode, m_is_paused);
}

void EngineControls::exit_play_mode() {
    EngineEventBus::get().publish<bool>(EngineEvent::ExitPlayMode, true);
}
