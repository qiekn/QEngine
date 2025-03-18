#include "console/console.h"

ConsoleWindow::ConsoleWindow() {
}

ImVec4 ConsoleWindow::get_log_level_color(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE:   return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        case LogLevel::INFO:    return ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
        case LogLevel::WARNING: return ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        case LogLevel::ERROR:   return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        default:                return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void ConsoleWindow::render(float y_position, float width, float height) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    ImGui::SetNextWindowPos(ImVec2(0, y_position));
    ImGui::SetNextWindowSize(ImVec2(width, height));

    if (ImGui::Begin("Console", nullptr, flags)) {
        if (ImGui::Button("Clear")) {
            Logger::get().clear();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_auto_scroll);

        ImGui::SameLine();

        ImGui::SameLine();
        float line_height = ImGui::GetTextLineHeight();
        ImVec2 cursor_pos = ImGui::GetCursorPos();
        ImVec2 p1 = ImVec2(cursor_pos.x + 4, cursor_pos.y);
        ImVec2 p2 = ImVec2(cursor_pos.x + 4, cursor_pos.y + line_height);
        ImGui::GetWindowDrawList()->AddLine(
            ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + line_height),
            ImGui::GetColorU32(ImGuiCol_Separator)
        );
        ImGui::SetCursorPos(ImVec2(cursor_pos.x + 8, cursor_pos.y)); // Add some spacing

        ImGui::SameLine();
        ImGui::Text("Show:");

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, get_log_level_color(LogLevel::TRACE));
        ImGui::Checkbox("Trace", &m_show_trace);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, get_log_level_color(LogLevel::INFO));
        ImGui::Checkbox("Info", &m_show_info);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, get_log_level_color(LogLevel::WARNING));
        ImGui::Checkbox("Warning", &m_show_warning);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, get_log_level_color(LogLevel::ERROR));
        ImGui::Checkbox("Error", &m_show_error);
        ImGui::PopStyleColor();

        ImGui::Separator();

        const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& logs = Logger::get().get_log_messages();

        for (const auto& log : logs) {
            LogLevel level = log.first;

            if ((level == LogLevel::TRACE && !m_show_trace) ||
                (level == LogLevel::INFO && !m_show_info) ||
                (level == LogLevel::WARNING && !m_show_warning) ||
                (level == LogLevel::ERROR && !m_show_error)) {
                continue;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, get_log_level_color(level));
            ImGui::TextUnformatted(log.second.c_str());
            ImGui::PopStyleColor();
        }

        if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::Separator();
        bool reclaim_focus = false;

        if (ImGui::InputText("Command", m_command_buffer, IM_ARRAYSIZE(m_command_buffer),
                            ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (m_command_buffer[0] != '\0') {
                log_info() << "Command: " << m_command_buffer;
                m_command_buffer[0] = '\0';
                reclaim_focus = true;
            }
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus) {
            ImGui::SetKeyboardFocusHere(-1);
        }

        ImGui::End();
    }
}
