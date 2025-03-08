#include <raylib.h>

#include "core/zeytin.h" // IWYU pragma: keep
#include "game/register.h"
#include "editor/editor_communication.h"

int main() {
    SetTraceLogLevel(LOG_ERROR);

    EditorCommunication editor;
    editor.initialize();

    Zeytin::get().init();

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib Test");

    SetTargetFPS(60);

    editor.notify_engine_started();
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            editor.process_messages();
            Zeytin::get().tick_variants();
            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}

