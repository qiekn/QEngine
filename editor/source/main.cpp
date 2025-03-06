#include <iostream> // IWYU pragma: keep
#include <thread>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <imgui.h>


#include "entity/entity_list.h"
#include "variant/variant_list.h"


#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"

#include "file_watcher/file_watcher.h"

#include "engine/engine_controls.h"

int main(int argc, char* argv[])
{
    EngineControls engine_controls;

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


    FileWatcher variant_watcher("../shared/variants/", std::chrono::milliseconds(500));
    variant_watcher.addCallback({ ".variant"}, [&variant_list](const fs::path& path, const std::string& status) {
        std::cout << "Variant file " << path << " was " << status << std::endl;
        variant_list.load_variant(path);
    });
    variant_watcher.start();

    FileWatcher entity_watcher("../shared/entities/", std::chrono::milliseconds(500));
    entity_watcher.addCallback({ ".entity" }, [&hierarchy, &entity_list](const fs::path& path, const std::string& status) -> void {
        if(hierarchy.ignore_file_events) {
            return;
        }

        std::cout << "Entity file " << path << " was " << status << std::endl;

        if (status == "created") {
            entity_list.load_entity_from_file(path);
        }
        else if (status == "modified") {
            std::string entity_name = path.stem().string();
            auto& entities = entity_list.get_entities();

            bool found = false;
            for (auto& entity : entities) {
                if (entity.get_name() == entity_name) {
                    entity.load_from_file();
                    found = true;
                    std::cout << "Reloaded entity: " << entity_name << std::endl;
                    break;
                }
            }
            if (!found) {
                std::cout << "Modified entity not found in list, loading as new: " << entity_name << std::endl;
                entity_list.load_entity_from_file(path);
            }
        }
        else if (status == "deleted") {
            std::string entity_name = path.stem().string();
            auto& entities = entity_list.get_entities();

            for (auto& entity : entities) {
                if (entity.get_name() == entity_name) {
                    entity.mark_as_dead();
                    std::cout << "Marked entity as dead: " << entity_name << std::endl;
                    break;
                }
            }
        }
    });

    entity_watcher.start();

	while (!WindowShouldClose())    
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		rlImGuiBegin();

        engine_controls.renderMainMenuControls();
        hierarchy.update();

		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}

    rlImGuiShutdown();
	CloseWindow(); 

	return 0;
}

