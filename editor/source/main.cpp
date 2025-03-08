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
    SetTraceLogLevel(LOG_ERROR); // raylib

    EngineControls engine_controls;
    EngineCommunication engine_communication;
    engine_communication.initialize();

	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Zeytin");
	SetTargetFPS(144);
	rlImGuiSetup(false);

    SetEditorTheme();

    EntityList entity_list;
    entity_list.load_entities();

    VariantList variant_list;
    variant_list.load_variants();

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());

    int frame_count = 0;

	while (!WindowShouldClose())    
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		rlImGuiBegin();

        engine_communication.process_recieved_messages();

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
