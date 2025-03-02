#include "raylib.h"

#include "imgui.h"
#include "rlImGui.h"
#include "rapidjson/document.h"

// DPI scaling functions
float ScaleToDPIF(float value)
{
    return GetWindowScaleDPI().x * value;
}

int ScaleToDPII(int value)
{
    return int(GetWindowScaleDPI().x * value);
}

void DisplayJson(rapidjson::Value& jsonValue) {
}

int main(int argc, char* argv[])
{
    const char* json = R"({
        "entity_id": 1,
        "variants": [
            {
                "type": "Position",
                "value": { "x": 2, "y": 4 }
            },
            {
                "type": "Velocity",
                "value": { "x": 50, "y": 60 }
            }
        ]
    })";



	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - simple ImGui Demo");
	SetTargetFPS(144);
	rlImGuiSetup(true);

	Texture image = LoadTexture("resources/parrots.png");

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		// start ImGui Conent
		rlImGuiBegin();

		// show ImGui Content
		bool open = true;
		ImGui::ShowDemoWindow(&open);

		open = true;
		if (ImGui::Begin("Test Window", &open))
		{
			ImGui::TextUnformatted(ICON_FA_JEDI);

			rlImGuiImage(&image);
		}
		ImGui::End();

		// end ImGui Content
		rlImGuiEnd();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	rlImGuiShutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
    rlImGuiShutdown();
	UnloadTexture(image);
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
