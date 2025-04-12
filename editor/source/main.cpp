#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "entity/entity_list.h"
#include "variant/variant_list.h"

#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"

#include "engine/engine_controls.h"
#include "engine/engine_communication.h"

#include "console/console.h"
#include "window/window_manager.h"
#include "asset_browser/asset_browser.h"

#include "imgui_test_engine/imgui_te_engine.h"
#include "imgui_test_engine/imgui_te_context.h"

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
    rlImGuiSetup(false);

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


    ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;

    //RegisterMyTests(engine); // will call IM_REGISTER_TEST() etc.

    //ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());

    //ImGuiTestEngine_InstallCrashHandler();

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

        rlImGuiEnd();
        EndDrawing();
    }

    engine_controls.kill_engine();

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
