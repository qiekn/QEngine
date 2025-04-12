#include "raylib.h"
#include "rlImGui.h"

#include "entity/entity_list.h"
#include "variant/variant_list.h"

#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"

#include "engine/engine_controls.h"
#include "engine/engine_communication.h"

#include "console/console.h"
#include "asset_browser/asset_browser.h"
#include "test_viewer/test_viewer.h"
#include "test_manager/test_manager.h"
#include "window/window_manager.h"


void RegisterEditorTests(ImGuiTestEngine* test_engine);

int main(int argc, char* argv[])
{
    SetTraceLogLevel(LOG_ERROR);

    EngineControls engine_controls;
    EngineCommunication engine_communication;

    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(screen_width, screen_height, "ZeytinEditor");
    MaximizeWindow();
    SetTargetFPS(144);
    SetExitKey(0);
    rlImGuiSetup(true);

    SetEditorTheme();

    EntityList entity_list{};
    VariantList variant_list{};

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());

    WindowManager window_manager;
    
    window_manager.set_hierarchy_render_func([&hierarchy]() {
        hierarchy.update();
    });
    
    window_manager.set_console_render_func([](float y_position, float width, float height) {
        ConsoleWindow::get().render(y_position, width, height);
    });

    window_manager.set_asset_browser_render_func([]() {
        AssetBrowser::get().render();
    });


    TestViewer test_viwer;

    window_manager.set_test_viewer_render_func([&test_viwer]() {
            test_viwer.render();
    });


    //ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext();
    //ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
    //test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    //test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    //test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Cinematic; 

    //ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
    //ImGuiTestEngine_InstallDefaultCrashHandler();
    //RegisterEditorTests(engine); 

    bool started = false;

    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_H)) {
            if(IsWindowMinimized()) {
                SetWindowFocused();
            }
            else {
                MinimizeWindow();
            }
        }

        SetWindowState(FLAG_WINDOW_UNFOCUSED);
        BeginDrawing();
        ClearBackground(BLACK);


        rlImGuiBegin();

        engine_controls.render_main_menu_controls();
        window_manager.render();

        //ImGuiTestEngine_ShowTestEngineWindows(engine, nullptr);

        rlImGuiEnd();
        EndDrawing();

        //ImGuiTestEngine_PostSwap(engine);
    }

    engine_controls.kill_engine();

    //ImGuiTestEngine_Stop(engine);
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}

void RegisterEditorTests(ImGuiTestEngine* test_engine)
{
    ImGuiTest* hierarchyTest = IM_REGISTER_TEST(test_engine, "Hierarchy" , "CreateEntity");
    hierarchyTest->TestFunc = [](ImGuiTestContext* ctx) {
        ctx->SetRef("Hierarchy");
        ctx->ItemClick("+ Create New Entity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("##EntityName");
        ctx->KeyCharsAppend("TestEntity");
        ctx->SetRef("New Entity");
        ctx->ItemClick("Create");
    };
}


