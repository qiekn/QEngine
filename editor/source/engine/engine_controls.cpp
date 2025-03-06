#include "engine/engine_controls.h"
#include <iostream>
#include <cstdlib>

EngineControls::EngineControls() 
    : m_isCompiling(false)
    , m_isRunning(false) {
}

void EngineControls::renderMainMenuControls() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Open file code */ }
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Save file code */ }
            if (ImGui::MenuItem("Exit", "Alt+F4")) { /* Exit code */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() * 0.3f);

        if (ImGui::Button("Compile Engine")) {
            if (!m_isCompiling) {
                compileEngine();
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Start Engine")) {
            if (!m_isRunning && !m_isCompiling) {
                startEngine();
            }
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        if (m_isCompiling) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Compiling...");
        } else if (m_isRunning) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Engine Running");
        }

        ImGui::EndMainMenuBar();
    }
}

void EngineControls::compileEngine() {
    m_isCompiling = true;

    std::thread([this]() {
        #ifdef _WIN32
            int result = std::system("cd ../runtime && build.sh");
        #else
            int result = std::system("cd ../runtime && ./build.sh");
        #endif

        if (result == 0) {
            std::cout << "Compilation succeeded" << std::endl;
        } else {
            std::cerr << "Compilation failed with code " << result << std::endl;
        }
        m_isCompiling = false;
    }).detach();
}

void EngineControls::startEngine() {
    m_isRunning = true;

    std::thread([this]() {
        #ifdef _WIN32
            int result = std::system("cd ../runtime && run.sh");
        #else
            int result = std::system("cd ../runtime && ./run.sh");
        #endif

        if (m_isRunning) {
            std::cout << "Engine process exited with code " << result << std::endl;
            m_isRunning = false;
        }
    }).detach();
}
