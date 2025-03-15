#include <iostream> // IWYU pragma: keep

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "entity/entity_list.h"
#include "variant/variant_list.h"

#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"

#include "engine/engine_controls.h"
#include "engine/engine_communication.h"

int main(int argc, char* argv[])
{
    SetTraceLogLevel(LOG_ERROR);

    EngineControls engine_controls;
    EngineCommunication engine_communication;

    int screenWidth = 1280;
    int screenHeight = 800;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "ZeytinEditor");
    SetTargetFPS(144);
    SetExitKey(0);
    rlImGuiSetup(false);

    SetEditorTheme();

    EntityList entity_list{};
    VariantList variant_list{};

    std::cout << entity_list.as_string() << std::endl;

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        engine_controls.render_main_menu_controls();
        hierarchy.update();

        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
