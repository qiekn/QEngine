#include <iostream> // IWYU pragma: keep

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <imgui.h>


#include "entity/entity_list.h"
#include "variant/variant_list.h"


#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"

int main(int argc, char* argv[])
{
	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Zeytin");
	SetTargetFPS(144);
	rlImGuiSetup(true);

    SetEditorTheme();

    EntityList entity_list;
    entity_list.load_entities();

    VariantList variant_list;
    variant_list.load_variants();

    Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());

    for(auto& variant : variant_list.get_variants()) {
        std::cout << "name: " << variant.get_name() << std::endl;
    }


	while (!WindowShouldClose())    
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		rlImGuiBegin();

        hierarchy.update();

		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}

    rlImGuiShutdown();
	CloseWindow(); 

	return 0;
}
