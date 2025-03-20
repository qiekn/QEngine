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

int main(int argc, char* argv[])
{
    SetTraceLogLevel(LOG_ERROR);

    EngineControls engine_controls;
    EngineCommunication engine_communication;

    int screenWidth = 1280;
    int screenHeight = 800;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "ZeytinEditor");
    MaximizeWindow();
    SetTargetFPS(144);
    SetExitKey(0);
    rlImGuiSetup(false);

    SetEditorTheme();

    EntityList entity_list{};
    VariantList variant_list{};

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());

    WindowManager windowManager;
    
    windowManager.set_hierarchy_render_func([&hierarchy]() {
        hierarchy.update();
    });
    
    windowManager.set_content_render_func([]() {
        ImGui::Text("Embeded scene");
    });
    
    windowManager.set_console_render_func([](float y_position, float width, float height) {
        ConsoleWindow::get().render(y_position, width, height);
    });

    windowManager.set_asset_browser_render_func([]() {
        AssetBrowser::get().render();
    });

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        engine_controls.render_main_menu_controls();
        windowManager.render();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
