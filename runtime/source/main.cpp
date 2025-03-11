#include <raylib.h>

#include "core/zeytin.h" // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep

#include "game/register.h"
#include "editor/editor_communication.h"


int main() {
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST);

    const int virtualWidth = 1920;
    const int virtualHeight = 1080;

    const int windowWidth = 800;
    const int windowHeight = 600;

    InitWindow(windowWidth, windowHeight, "");
    SetWindowPosition(1051, 393);
    SetExitKey(0);

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);

    float scaleX = (float)windowWidth / virtualWidth;
    float scaleY = (float)windowHeight / virtualHeight;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    Vector2 position = {
        (windowWidth - (virtualWidth * scale)) * 0.5f,
        (windowHeight - (virtualHeight * scale)) * 0.5f
    };

    SetTargetFPS(60);

    EditorCommunication editor_comm;
    editor_comm.initialize();

    Zeytin::get().init();

    while (!WindowShouldClose()) {
        editor_comm.heartbeet();

        Vector2 mousePosition = GetMousePosition();

        Vector2 virtualMousePosition = {
            (mousePosition.x - position.x) / scale,
            (mousePosition.y - position.y) / scale
        };


        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            DrawText("1920x1080", 20, 20, 40, BLACK);
            DrawCircle(960, 540, 100, RED);

            editor_comm.raise_events();
            Zeytin::get().tick_variants();

            DrawCircle(virtualMousePosition.x, virtualMousePosition.y, 10, GREEN);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            DrawTexturePro(
                target.texture,
                (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
                (Rectangle){ position.x, position.y, virtualWidth * scale, virtualHeight * scale },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            DrawFPS(windowWidth - 80, windowHeight - 30);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
