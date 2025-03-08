#include "engine/engine_controls.h"
#include <iostream>
#include <cstdlib>

#include "engine/engine_event.h"

EngineControls::EngineControls() 
    : m_compiled(false)
    , m_is_running(false) {

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

        if(!m_is_running) {
            if (ImGui::Button("Start Engine")) {
                start_engine();
            }
        }
        else {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Engine Running");
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












