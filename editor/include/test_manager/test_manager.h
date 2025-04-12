#pragma once

#include "imgui_test_engine/imgui_te_engine.h"
#include "imgui_test_engine/imgui_te_context.h"
#include "imgui_test_engine/imgui_te_ui.h"

class TestManager {
public:
    static TestManager& get() {
        static TestManager instance;
        return instance;
    }

    void initialize();
    void update();
    void shutdown();
    
private:
    TestManager() = default;
    ~TestManager() = default;

    void register_all_tests();

    ImGuiTestEngine* m_test_engine = nullptr;
};
