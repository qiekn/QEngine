#include "engine/engine_controls.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "imgui.h"
#include "rapidjson/document.h"

#include "logger.h"
#include "engine/engine_event.h"

EngineControls::EngineControls() 
    : m_is_running(false)
    , m_is_play_mode(false)
    , m_is_paused(false)
    , m_is_engine_starting(false)
    , m_build_status(BuildStatus::None)
    , m_build_monitor_active(false)
{
    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EngineStarted,
        [this](const bool& success) {
            if (success) {
                m_is_running = true;
                m_is_engine_starting = false;
                m_build_status = BuildStatus::None;
                log_info() << "Engine started successfully." << std::endl;
            } 
        }
    );

    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EngineStopped,
        [this](auto _) {
            if(m_is_play_mode) {
                exit_play_mode();
            }
            m_is_running = false;
            m_is_engine_starting = false;
        }
    );
    
    std::filesystem::create_directories("../runtime/build_status");
    
    std::string status_file = "../runtime/build_status/build_status.json";
    if (std::filesystem::exists(status_file)) {
        try {
            std::ofstream file(status_file);
            if (file.is_open()) {
                file << "{\"status\":\"none\",\"message\":\"Editor started\",\"timestamp\":\"" << std::time(nullptr) << "\"}";
                file.close();
            }
        } catch (...) {
            try {
                std::filesystem::remove(status_file);
            } catch (...) {
            }
        }
    }
}

EngineControls::~EngineControls() {
    m_build_monitor_active = false;
    if (m_build_monitor_future.valid()) {
        m_build_monitor_future.wait();
    }
    kill_engine();
}

void EngineControls::render() {
    check_build_status();

    float total_width = ImGui::GetWindowWidth();
    float center_width = total_width * 0.2;
    float center_pos = (total_width - center_width) * 0.5f;
    float right_pos = total_width - 400; 

    ImGui::SetCursorPosX(center_pos);
    render_engine_controls();

    ImGui::SetCursorPosX(right_pos);
    render_play_controls();

    if (m_build_status == BuildStatus::Running || m_build_status == BuildStatus::Failed) {
        render_build_status();
    }
}

void EngineControls::render_engine_controls() {
    if (ImGui::Button("Kill Engine")) {
        kill_engine();
    }

    if (!m_is_running) {
        ImGui::SameLine();
        if (!m_is_engine_starting) {
            if (m_build_status == BuildStatus::Running) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                ImGui::Button("Building...");
                ImGui::PopStyleColor(4);
            } else if (ImGui::Button("Start Engine")) {
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
    
    ImGui::SameLine(ImGui::GetWindowWidth() - 120);
    switch (m_build_status) {
        case BuildStatus::Running:
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Building...");
            break;
        case BuildStatus::Success:
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Build OK");
            break;
        case BuildStatus::Failed:
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Build Failed!");
            break;
        default:
            break;
    }
}

void EngineControls::render_play_controls() {
    if (m_is_running) {
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

void EngineControls::render_build_status() {
    if (m_build_status == BuildStatus::Failed) {
        m_is_engine_starting = false;
        m_is_running = false;
    }
    else if (m_build_status == BuildStatus::Running) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 210, 50));
        ImGui::SetNextWindowSize(ImVec2(200, 80));
        ImGui::Begin("Building", nullptr, 
                    ImGuiWindowFlags_NoTitleBar | 
                    ImGuiWindowFlags_NoResize | 
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse);
        
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Building Runtime...");
        
        float progress = (float)(ImGui::GetTime() * 0.5f);
        progress = progress - (int)progress;
        
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
        
        ImGui::End();
    }
}

void EngineControls::check_build_status() {
    std::string status_file = "../runtime/build_status/build_status.json";
    
    if (!std::filesystem::exists(status_file)) {
        return;
    }
    
    try {
        std::ifstream file(status_file);
        if (!file.is_open()) {
            return;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json_string = buffer.str();
        file.close();
        
        rapidjson::Document doc;
        doc.Parse(json_string.c_str());
        
        if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("status")) {
            return;
        }
        
        std::string status = doc["status"].GetString();
        std::string message = doc.HasMember("message") ? doc["message"].GetString() : "";
        
        if (status == "none") {
            return;
        }
        
        if (status == "running" && m_build_status != BuildStatus::Running) {
            m_build_status = BuildStatus::Running;
            log_info() << "Build status: RUNNING - " << message << std::endl;
            
            static std::string build_log_path = "../runtime/build_status/build_output.log";
            if (std::filesystem::exists(build_log_path)) {
                static std::ifstream log_file(build_log_path);
                if (log_file.is_open()) {
                    std::string line;
                    while (std::getline(log_file, line)) {
                        log_info() << "BUILD: " << line << std::endl;
                    }
                }
            }
        }
        else if (status == "success" && m_build_status != BuildStatus::Success) {
            m_build_status = BuildStatus::Success;
            
            static float success_timer = 0.0f;
            if (m_build_status == BuildStatus::Success) {
                success_timer += ImGui::GetIO().DeltaTime;
                if (success_timer > 3.0f) {
                    m_build_status = BuildStatus::None;
                    success_timer = 0.0f;
                }
            }
        }
        else if (status == "failed" && m_build_status != BuildStatus::Failed) {
            m_build_status = BuildStatus::Failed;
            
            if (doc.HasMember("message")) {
                m_build_message = doc["message"].GetString();
            }
            
            if (doc.HasMember("details")) {
                m_build_details = doc["details"].GetString();
                log_error() << "Build status: FAILED - " << m_build_message << std::endl;
                log_error() << "Details: " << m_build_details << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        log_error() << "Error reading build status: " << e.what() << std::endl;
    }
}

void EngineControls::start_engine() {
    log_info() << "Attempt to start the engine..." << std::endl;
    
    m_build_status = BuildStatus::Running;
    m_build_message.clear();
    m_build_details.clear();
    
    std::filesystem::create_directories("../runtime/build_status");
    
    monitor_build();
}

void EngineControls::monitor_build() {
    if (m_build_monitor_future.valid()) {
        m_build_monitor_active = false;
        m_build_monitor_future.wait();
    }
    
    m_build_monitor_active = true;
    log_info() << "Starting build process..." << std::endl;
    
    m_build_monitor_future = std::async(std::launch::async, [this]() {
        std::filesystem::remove_all("../runtime/build_status");
        std::filesystem::create_directory("../runtime/build_status");
        
        #ifdef _WIN32
        log_info() << "Executing: cd ../runtime && build_wrapper.sh build_status/build_status.json" << std::endl;
        int result = std::system("cd ../runtime && build_wrapper.sh build_status/build_status.json");
        #else
        log_info() << "Executing: cd ../runtime && ./build_wrapper.sh build_status/build_status.json" << std::endl;
        int result = std::system("cd ../runtime && ./build_wrapper.sh build_status/build_status.json");
        #endif
        
        if (result != 0) {
            log_error() << "Build command failed with exit code: " << result << std::endl;
            std::ofstream status_file("../runtime/build_status/build_status.json");
            if (status_file.is_open()) {
                status_file << "{\"status\":\"failed\",\"message\":\"Build process failed with code " << result << "\",\"timestamp\":\"" << std::time(nullptr) << "\"}";
                status_file.close();
            }
            
            try {
                std::ifstream log_file("../runtime/build_status/build_output.log");
                if (log_file.is_open()) {
                    std::string line;
                    std::vector<std::string> last_lines;
                    while (std::getline(log_file, line)) {
                        last_lines.push_back(line);
                        if (last_lines.size() > 10) {
                            last_lines.erase(last_lines.begin());
                        }
                    }
                    log_file.close();
                    
                    log_error() << "Last " << last_lines.size() << " lines of build log:" << std::endl;
                    for (const auto& line : last_lines) {
                        log_error() << line << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                log_error() << "Failed to read build log: " << e.what() << std::endl;
            }
        } else {
            std::ofstream status_file("../runtime/build_status/build_status.json");
            if (status_file.is_open()) {
                status_file << "{\"status\":\"success\",\"message\":\"Build completed successfully\",\"timestamp\":\"" << std::time(nullptr) << "\"}";
                status_file.close();
            }
            
            log_info() << "Launching engine..." << std::endl;
            #ifdef _WIN32
            std::system("cd ../runtime && run.sh");
            #else
            std::system("cd ../runtime && ./run.sh");
            #endif
        }
    });
}

void EngineControls::kill_engine() {
    log_info() << "Killing the engine..." << std::endl;

    m_is_play_mode = false;
    m_is_running = false;
    m_is_paused = false;
    m_is_engine_starting = false;

    EngineEventBus::get().publish<bool>(EngineEvent::KillEngine, true);
}

void EngineControls::enter_play_mode() {
    EngineEventBus::get().publish<bool>(EngineEvent::EnterPlayMode, m_is_paused);
}

void EngineControls::exit_play_mode() {
    m_is_play_mode = false;
    m_is_paused = false;
    EngineEventBus::get().publish<bool>(EngineEvent::ExitPlayMode, true);
}
