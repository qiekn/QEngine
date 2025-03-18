#include "window/window_manager.h"
#include "raylib.h"

WindowManager::WindowManager() 
    : m_hierarchyRenderFunc([]() {}),  
      m_contentRenderFunc([]() {}),
      m_consoleRenderFunc([](float, float, float) {}) {
}

void WindowManager::drawResizeHandle(ImVec2 start, ImVec2 end, bool isHorizontal, bool isHovered) {
    ImGui::GetForegroundDrawList()->AddRectFilled(
        start, end, 
        isHovered ? IM_COL32(100, 100, 100, 255) : IM_COL32(80, 80, 80, 255)
    );
    
    const int numLines = isHorizontal ? 3 : 5;
    const float lineLength = isHorizontal ? (end.y - start.y) * 0.4f : (end.x - start.x) * 0.4f;
    const float spacing = isHorizontal ? (end.x - start.x) / (numLines + 1) : (end.y - start.y) / (numLines + 1);
    const ImVec2 center = ImVec2((start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f);
    
    ImU32 lineColor = isHovered ? IM_COL32(220, 220, 220, 200) : IM_COL32(180, 180, 180, 150);
    
    if (isHorizontal) {
        for (int i = 0; i < numLines; i++) {
            float x = start.x + spacing * (i + 1);
            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(x, center.y - lineLength/2),
                ImVec2(x, center.y + lineLength/2),
                lineColor, 1.0f
            );
        }
    } else {
        for (int i = 0; i < numLines; i++) {
            float y = start.y + spacing * (i + 1);
            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(center.x - lineLength/2, y),
                ImVec2(center.x + lineLength/2, y),
                lineColor, 1.0f
            );
        }
    }
}

void WindowManager::render() {
    float menuBarHeight = ImGui::GetFrameHeight();
    ImVec2 windowSize = ImVec2(GetScreenWidth(), GetScreenHeight());
    
    float mainContentHeight = windowSize.y - menuBarHeight - m_consoleHeight;

    if (m_hierarchyWidth < m_hierarchyMinWidth) m_hierarchyWidth = m_hierarchyMinWidth;
    if (m_hierarchyWidth > m_hierarchyMaxWidth) m_hierarchyWidth = m_hierarchyMaxWidth;
    if (m_consoleHeight < m_consoleMinHeight) m_consoleHeight = m_consoleMinHeight;
    if (m_consoleHeight > m_consoleMaxHeight) m_consoleHeight = m_consoleMaxHeight;

    ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(m_hierarchyWidth, mainContentHeight));
    ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | 
                                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin("Hierarchy", nullptr, hierarchyFlags)) {
        m_hierarchyRenderFunc();
        ImGui::End();
    }

    const float resizeHandleWidth = 12.0f;
    ImVec2 resizeStart(m_hierarchyWidth - resizeHandleWidth/2, menuBarHeight);
    ImVec2 resizeEnd(m_hierarchyWidth + resizeHandleWidth/2, menuBarHeight + mainContentHeight);
    
    bool isHoverHierarchyResize = ImGui::IsMouseHoveringRect(resizeStart, resizeEnd);
    
    if (isHoverHierarchyResize) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        
        if (ImGui::IsMouseClicked(0)) {
            m_isResizingHierarchy = true;
        }
    }
    
    if (m_isResizingHierarchy) {
        m_hierarchyWidth = ImGui::GetIO().MousePos.x;
        
        if (m_hierarchyWidth < m_hierarchyMinWidth) m_hierarchyWidth = m_hierarchyMinWidth;
        if (m_hierarchyWidth > m_hierarchyMaxWidth) m_hierarchyWidth = m_hierarchyMaxWidth;
        
        if (!ImGui::IsMouseDown(0)) {
            m_isResizingHierarchy = false;
        }
        
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    
    drawResizeHandle(resizeStart, resizeEnd, false, isHoverHierarchyResize);

    ImGui::SetNextWindowPos(ImVec2(m_hierarchyWidth + resizeHandleWidth/2, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(windowSize.x - m_hierarchyWidth - resizeHandleWidth/2, mainContentHeight));
    ImGuiWindowFlags contentFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | 
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    
    if (ImGui::Begin("Content", nullptr, contentFlags)) {
        m_contentRenderFunc();
        ImGui::End();
    }
    
    float consoleY = menuBarHeight + mainContentHeight;
    const float consoleResizeHeight = 16.0f;
    ImVec2 consoleResizeStart(0, consoleY - consoleResizeHeight/2);
    ImVec2 consoleResizeEnd(windowSize.x, consoleY + consoleResizeHeight/2);
    
    bool isHoverConsoleResize = ImGui::IsMouseHoveringRect(consoleResizeStart, consoleResizeEnd);
    
    if (isHoverConsoleResize) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        
        if (ImGui::IsMouseClicked(0)) {
            m_isResizingConsole = true;
        }
    }
    
    if (m_isResizingConsole) {
        float mouseY = ImGui::GetIO().MousePos.y;
        float newConsoleHeight = windowSize.y - mouseY;
        
        if (newConsoleHeight < m_consoleMinHeight) newConsoleHeight = m_consoleMinHeight;
        if (newConsoleHeight > m_consoleMaxHeight) newConsoleHeight = m_consoleMaxHeight;
        
        m_consoleHeight = newConsoleHeight;
        mainContentHeight = windowSize.y - menuBarHeight - m_consoleHeight;
        consoleY = menuBarHeight + mainContentHeight;
        
        if (!ImGui::IsMouseDown(0)) {
            m_isResizingConsole = false;
        }
        
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    
    drawResizeHandle(consoleResizeStart, consoleResizeEnd, true, isHoverConsoleResize);
    
    m_consoleRenderFunc(consoleY, windowSize.x, m_consoleHeight);
}
