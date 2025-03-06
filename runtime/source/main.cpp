#include <raylib.h>

#include "core/zeytin.h"
#include "game/register.h"
#include "file_watcher/file_watcher.h"

int main() {
    Zeytin::get().init();

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib Test");

    SetTargetFPS(60);

    FileWatcher entity_watcher("../shared/entities/", std::chrono::milliseconds(500));
    entity_watcher.addCallback({ ".entity" }, [](const fs::path& path, const std::string& status) -> void {
        std::cout << "Entity file " << path << " was " << status << std::endl;

        if (status == "created") {
            Zeytin::get().deserialize_entity(path);
        }
        else if (status == "modified") {
            Zeytin::get().deserialize_entity(path);
        }
        else if (status == "deleted") {
            std::cout << "Not implemented deleted status" << std::endl;
        }
    });

    entity_watcher.start();

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            Zeytin::get().tick_variants();
            DrawFPS(10, 10);
        EndDrawing();
    }

    //CloseWindow();

    return 0;
}

