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

int main(int argc, char* argv[])
{
    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_WINDOW_ALWAYS_RUN);
    
    InitWindow(1280, 720, "ZeytinEditor");
    
    SetTargetFPS(60);
    
    SetExitKey(0);
    
    rlImGuiSetup(true);
    SetEditorTheme();

    EngineControls engine_controls;
    EngineCommunication engine_communication;

    EntityList entity_list{};
    VariantList variant_list{};

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());
    TestViewer test_viewer;

    WindowManager window_manager;
    
    window_manager.add_menu_item("Hierarchy", 
        [&hierarchy]() {
            hierarchy.update();
        });
    
    window_manager.add_menu_item("Console", 
        []() {
            ConsoleWindow::get().render();
        });
    
    window_manager.add_menu_item("Asset Browser", 
        []() {
            AssetBrowser::get().render();
        });

    //window_manager.add_menu_item("", "", [&engine_controls] {
    //        engine_controls.render();
    //    });

    //TestManager test_manager;

    //window_manager.add_menu_item("Tests", "Automated Tests", [&test_manager] {
    //        test_manager.update();
    //});

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        window_manager.render();

        rlImGuiEnd();
        EndDrawing();

        //test_manager.post_swap(); // required to have it for now
    }

    engine_controls.kill_engine();

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
