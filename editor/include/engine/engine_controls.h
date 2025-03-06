#pragma once

#include <atomic>
#include <thread>
#include <imgui.h>

class EngineControls {
public:
    EngineControls();
    ~EngineControls() = default;

    void renderMainMenuControls();

private:
    std::atomic<bool> m_isCompiling;
    std::atomic<bool> m_isRunning;

    void compileEngine();
    void startEngine();
};
