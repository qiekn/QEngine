#include "engine/engine_controls.h"
#include <iostream>
#include <cstdlib>

#include "engine/engine_event.h"

EngineControls::EngineControls() 
    : m_is_running(false), m_is_play_mode(false) {

        EngineEventBus::get().subscribe<bool>(EngineEvent::EngineStarted, [this](const bool& success) {
        if (success) {
            m_is_running = true;
        }
    });
}
void EngineControls::render_main_menu_controls() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {} 
            if (ImGui::MenuItem("Save", "Ctrl+S")) {} 
            if (ImGui::MenuItem("Exit", "Alt+F4")) {} 
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }                  

        ImGui::SameLine(ImGui::GetWindowWidth() * 0.4f);
  
        // Engine status display
        if(!m_is_running) {
            if (ImGui::Button("Start Engine")) {
                start_engine();
            }              
        }                  
        else {             
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Engine Running");
        }
        
        // Play mode section (Unity-like)
        ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
        
        // Play mode buttons with colors similar to Unity's play mode controls
        // Only enable play controls if engine is running
        if (m_is_running) {
            // Change Play button color based on play mode state
            ImGui::PushStyleColor(ImGuiCol_Button, m_is_play_mode ? ImVec4(0.0f, 0.5f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_is_play_mode ? ImVec4(0.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_is_play_mode ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            
            if (ImGui::Button("Play")) {
                m_is_play_mode = !m_is_play_mode;
                if (m_is_play_mode) {
                    //enter_play_mode();
                    m_is_paused = false;
                } else {
                    //exit_play_mode();
                    m_is_paused = false;
                }
            }
            ImGui::PopStyleColor(3);
            
            ImGui::SameLine();
            // Change Pause button color based on paused state
            ImGui::PushStyleColor(ImGuiCol_Button, m_is_paused ? ImVec4(0.8f, 0.5f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_is_paused ? ImVec4(0.9f, 0.6f, 0.0f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_is_paused ? ImVec4(1.0f, 0.7f, 0.0f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            
            if (ImGui::Button("Pause")) {
                //toggle_pause();
                m_is_paused = !m_is_paused;
            }
            ImGui::PopStyleColor(3);
            
            // Display play mode status text
            ImGui::SameLine();
            if (m_is_play_mode) {
                if (m_is_paused) {
                    ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "PAUSED");
                } else {
                    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "PLAYING");
                }
            } else {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "STOPPED");
            }
        } else {
            // Display disabled play controls when engine is not running
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            
            ImGui::Button("Play");
            ImGui::SameLine();
            ImGui::Button("Pause");
            
            ImGui::PopStyleColor(4);
        }

        ImGui::EndMainMenuBar();        
    }
}

void EngineControls::start_engine() {
    std::thread([this]() {
        #ifdef _WIN32
            int result = std::system("cd ../runtime && build.sh && run.sh");
        #else
            int result = std::system("cd ../runtime && ./build.sh && ./run.sh");
        #endif

        if (m_is_running) {
            std::cout << "Engine process exited with code " << result << std::endl;
            m_is_running = false;
        }
    }).detach();
}












