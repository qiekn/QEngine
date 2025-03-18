#include "console/console.h"

ConsoleWindow::ConsoleWindow() {
}

ImVec4 ConsoleWindow::getLogLevelColor(LogLevel level) const {
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
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);

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
        ImGui::PushStyleColor(ImGuiCol_Text, getLogLevelColor(LogLevel::TRACE));
        ImGui::Checkbox("Trace", &m_showTrace);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, getLogLevelColor(LogLevel::INFO));
        ImGui::Checkbox("Info", &m_showInfo);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, getLogLevelColor(LogLevel::WARNING));
        ImGui::Checkbox("Warning", &m_showWarning);
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, getLogLevelColor(LogLevel::ERROR));
        ImGui::Checkbox("Error", &m_showError);
        ImGui::PopStyleColor();

        ImGui::Separator();

        const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& logs = Logger::get().getLogMessages();

        for (const auto& log : logs) {
            LogLevel level = log.first;

            if ((level == LogLevel::TRACE && !m_showTrace) ||
                (level == LogLevel::INFO && !m_showInfo) ||
                (level == LogLevel::WARNING && !m_showWarning) ||
                (level == LogLevel::ERROR && !m_showError)) {
                continue;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, getLogLevelColor(level));
            ImGui::TextUnformatted(log.second.c_str());
            ImGui::PopStyleColor();
        }

        if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::Separator();
        bool reclaimFocus = false;

        if (ImGui::InputText("Command", m_commandBuffer, IM_ARRAYSIZE(m_commandBuffer),
                            ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (m_commandBuffer[0] != '\0') {
                log_info() << "Command: " << m_commandBuffer;
                m_commandBuffer[0] = '\0';
                reclaimFocus = true;
            }
        }

        ImGui::SetItemDefaultFocus();
        if (reclaimFocus) {
            ImGui::SetKeyboardFocusHere(-1);
        }

        ImGui::End();
    }
}
