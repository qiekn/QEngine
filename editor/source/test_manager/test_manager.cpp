#include "test_manager/test_manager.h"

#include "imgui_test_engine/imgui_te_ui.h"
#include "logger.h"

TestManager::TestManager() {
    m_test_engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(m_test_engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Fast;
    test_io.ConfigMouseDrawCursor = true;

    ImGuiTestEngine_Start(m_test_engine, ImGui::GetCurrentContext());
    //ImGuiTestEngine_InstallDefaultCrashHandler(); // not very useful

    register_all_tests();
}

void TestManager::update() {
    if (m_test_engine) {
        ImGuiTestEngine_ShowTestEngineWindows(m_test_engine, &m_window_visible);
        ImGuiTestEngine_PostSwap(m_test_engine);
    }
}

TestManager::~TestManager() {
    if (m_test_engine) {
        ImGuiTestEngine_Stop(m_test_engine);
        ImGuiTestEngine_DestroyContext(m_test_engine);
        m_test_engine = nullptr;
    }
}

void TestManager::register_all_tests() {
    // ideally start engine before 

    ImGuiTest* hierarchy_create_entity = IM_REGISTER_TEST(m_test_engine, "Hierarchy" , "Create Entity");
    hierarchy_create_entity->TestFunc = [](ImGuiTestContext* ctx) {
        ctx->SetRef("Hierarchy");
        ctx->ItemClick("+ Create New Entity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("##EntityName");
        ctx->KeyCharsAppend("TestEntity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("Create");
        ctx->SetRef("Hierarchy");
        bool item_exists = ctx->ItemExists("**/TestEntity");
        IM_CHECK(item_exists);
    };

    ImGuiTest* hierarcy_cancel_create_entity = IM_REGISTER_TEST(m_test_engine, "Hierarchy" , "Cancel Create Entity");
    hierarcy_cancel_create_entity->TestFunc = [](ImGuiTestContext* ctx) {
        ctx->SetRef("Hierarchy");
        ctx->ItemClick("+ Create New Entity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("##EntityName");
        ctx->KeyCharsAppend("WillCancelThis");
        ctx->SetRef("New Entity");
        ctx->ItemClick("Cancel");
        bool item_exists = ctx->ItemExists("**/WillCancelThis");
        IM_CHECK(!item_exists);
    };

    ImGuiTest* add_position = IM_REGISTER_TEST(m_test_engine, "Hierarchy", "Add Position To Entity");
    add_position->TestFunc = [](ImGuiTestContext* ctx) {
        IM_CHECK(ctx->ItemExists("Hierarchy"));
        ctx->SetRef("Hierarchy");
        ctx->MouseMove("**/TestEntity");
        ctx->MouseClick(0);
        ctx->MouseClick(1);
        ctx->MouseMove("**/Add Variant");
        ctx->MouseMove("**/Position");
        ctx->MouseClick(0);
        bool item_exists = ctx->ItemExists("**/Position");
        IM_CHECK(item_exists);
    };

    ImGuiTest* re_add_position = IM_REGISTER_TEST(m_test_engine, "Hierarchy", "Re Add Position To Entity");
    add_position->TestFunc = [](ImGuiTestContext* ctx) {
        IM_CHECK(ctx->ItemExists("Hierarchy"));
        ctx->SetRef("Hierarchy");
        ctx->MouseMove("**/TestEntity");
        ctx->MouseClick(0);
        ctx->MouseClick(1);
        ctx->MouseMove("**/Add Variant");
        ctx->MouseMove("**/Position");
        ctx->MouseClick(0);
        bool item_exists = ctx->ItemExists("**/Position");
        IM_CHECK(item_exists);
    };

    ImGuiTest* save_entity_test = IM_REGISTER_TEST(m_test_engine, "Hierarchy", "Save Entity");
    save_entity_test->TestFunc = [](ImGuiTestContext* ctx) {
        ctx->SetRef("Hierarchy");
        IM_CHECK(ctx->ItemExists("Save All"));
        ctx->ItemClick("Save All");
    };

}
