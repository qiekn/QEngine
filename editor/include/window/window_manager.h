#pragma once

#include <functional>
#include "imgui.h"

class WindowManager {
public:
    WindowManager();
    ~WindowManager() = default;

    void render();

    void setHierarchyRenderFunc(std::function<void()> func) { m_hierarchyRenderFunc = func; }
    void setContentRenderFunc(std::function<void()> func) { m_contentRenderFunc = func; }
    void setConsoleRenderFunc(std::function<void(float, float, float)> func) { m_consoleRenderFunc = func; }

    float getHierarchyWidth() const { return m_hierarchyWidth; }
    float getConsoleHeight() const { return m_consoleHeight; }

private:
    void drawResizeHandle(ImVec2 start, ImVec2 end, bool isHorizontal, bool isHovered);

    float m_hierarchyWidth = 300.0f;
    float m_hierarchyMinWidth = 200.0f;
    float m_hierarchyMaxWidth = 500.0f;
    bool m_isResizingHierarchy = false;

    float m_consoleHeight = 300.0f;
    float m_consoleMinHeight = 300.0f;
    float m_consoleMaxHeight = 500.0f;
    bool m_isResizingConsole = false;

    std::function<void()> m_hierarchyRenderFunc;
    std::function<void()> m_contentRenderFunc;
    std::function<void(float, float, float)> m_consoleRenderFunc;
};
