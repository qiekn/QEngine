#include <raylib.h>

#include "core/zeytin.h"
#include "game/register.h"

int main() {
    Zeytin::get().init();

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib Test");

    SetTargetFPS(60);

    Zeytin::get().awake_variants();

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

