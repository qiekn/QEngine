#pragma once

#include "logger/logger.h"
#include "imgui.h"

class ConsoleWindow {
public:
    ConsoleWindow();

    void render(float y_position, float width, float height);

    void setShowTrace(bool show) { m_showTrace = show; }
    void setShowInfo(bool show) { m_showInfo = show; }
    void setShowWarning(bool show) { m_showWarning = show; }
    void setShowError(bool show) { m_showError = show; }

    void setAutoScroll(bool autoScroll) { m_autoScroll = autoScroll; }
    bool getAutoScroll() const { return m_autoScroll; }

    static ConsoleWindow& get() {
        static ConsoleWindow instance;
        return instance;
    }

private:
    bool m_showTrace = true;
    bool m_showInfo = true;
    bool m_showWarning = true;
    bool m_showError = true;

    bool m_autoScroll = true;

    char m_commandBuffer[256] = "";

    ImVec4 getLogLevelColor(LogLevel level) const;
};
