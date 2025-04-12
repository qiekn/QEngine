#include "test_manager/test_manager.h"

#include "imgui_test_engine/imgui_te_ui.h"

void TestManager::initialize() {
    m_test_engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(m_test_engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Cinematic;

    ImGuiTestEngine_Start(m_test_engine, ImGui::GetCurrentContext());
    ImGuiTestEngine_InstallDefaultCrashHandler();

    register_all_tests();
}

void TestManager::update() {
    if (m_test_engine) {
        ImGuiTestEngine_ShowTestEngineWindows(m_test_engine, nullptr);
        ImGuiTestEngine_PostSwap(m_test_engine);
    }
}

void TestManager::shutdown() {
    if (m_test_engine) {
        ImGuiTestEngine_Stop(m_test_engine);
        ImGuiTestEngine_DestroyContext(m_test_engine);
        m_test_engine = nullptr;
    }
}

void TestManager::register_all_tests() {
    ImGuiTest* hierarchy_create_entity = IM_REGISTER_TEST(m_test_engine, "Hierarchy" , "CreateEntity");
    hierarchy_create_entity->TestFunc = [](ImGuiTestContext* ctx) {
        ctx->SetRef("Hierarchy");
        ctx->ItemClick("+ Create New Entity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("##EntityName");
        ctx->KeyCharsAppend("TestEntity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("Create");
    };

    // Add more tests here as needed
    ImGuiTest* addVariantTest = IM_REGISTER_TEST(m_test_engine, "Hierarchy", "AddVariant");
    addVariantTest->TestFunc = [](ImGuiTestContext* ctx) {
        // First make sure we have an entity
        ctx->SetRef("Hierarchy");
        ctx->ItemClick("+ Create New Entity");
        ctx->SetRef("New Entity");
        ctx->KeyCharsAppend("VariantEntity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("Create");

        // Now add a variant to it
        ctx->SetRef("Hierarchy");
        //ctx->ItemContextClick("VariantEntity");
        ctx->MenuClick("Add Variant");
        // Complete with your specific UI flow for adding variants
    };
}
