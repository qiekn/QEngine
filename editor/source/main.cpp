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
#include "path_resolver/path_resolver.h"

void RegisterEditorTests(ImGuiTestEngine* test_engine);

int main(int argc, char* argv[])
{
    SetTraceLogLevel(LOG_ERROR);

    EngineControls engine_controls;
    EngineCommunication engine_communication;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "ZeytinEditor");
    MaximizeWindow();
    SetTargetFPS(144);
    SetExitKey(0);
    rlImGuiSetup(true);
    SetEditorTheme();

    EntityList entity_list{};
    VariantList variant_list{};

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());
    TestViewer test_viewer;

    WindowManager window_manager;
    
    window_manager.add_window("Hierarchy", 
        [&hierarchy](const ImVec2&, const ImVec2&) {
            hierarchy.update();
        });
    
    window_manager.add_window("Console", 
        [](const ImVec2& pos, const ImVec2& size) {
            ConsoleWindow::get().render(pos.y, size.x, size.y);
        });
    
    window_manager.add_window("Asset Browser", 
        [](const ImVec2&, const ImVec2&) {
            AssetBrowser::get().render();
        });

    window_manager.add_menu_item("", "", [&engine_controls] {
            engine_controls.render();
        });

    TestManager test_manager;

    window_manager.add_menu_item("Tests", "Automated Tests", [&test_manager] {
            test_manager.update();
    });

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

        window_manager.render();

        rlImGuiEnd();
        EndDrawing();

        test_manager.post_swap(); // required to have it for now
    }

    engine_controls.kill_engine();

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
