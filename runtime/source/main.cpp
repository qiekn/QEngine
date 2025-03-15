#include <raylib.h> // IWYU pragme: keep
#include <iostream> // IWYU pragma: keep

#include "core/zeytin.h" // IWYU pragma: keep
#include "editor/editor_communication.h" // IWYU pragma: keep
#include "game/rttr_registration.h"


int main(int argc, char* argv[]) {

#ifdef EDITOR_MODE 
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST);
    Zeytin::get().generate_variants();

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--kill-after-generate") == 0) {
            exit(0);
        }
    }

    const int windowWidth = 800;
    const int windowHeight = 600;

    InitWindow(windowWidth, windowHeight, "Game - ZeytinEngine");
#else
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    InitWindow(windowWidth, windowHeight, "Jam Game");

#endif

    const int virtualWidth = 1920;
    const int virtualHeight = 1080;

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

#ifdef EDITOR_MODE
    EditorCommunication editor_comm;
    editor_comm.initialize();
#endif

    Zeytin::get().init();

    while (!WindowShouldClose()) {


        Vector2 mousePosition = GetMousePosition();

        Vector2 virtualMousePosition = {
            (mousePosition.x - position.x) / scale,
            (mousePosition.y - position.y) / scale
        };

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            Zeytin::get().update_variants(); 

#ifdef EDITOR_MODE
            editor_comm.heartbeet();
            editor_comm.raise_events();


            Zeytin::get().sync_editor_play_mode(); 

            DrawText("1920x1080", 20, 20, 40, BLACK);
            DrawCircle(virtualMousePosition.x, virtualMousePosition.y, 10, GREEN);

            if(Zeytin::get().is_play_mode()) {
                if(Zeytin::get().is_paused_play_mode()) {
                    DrawText("PAUSED", 1610, 20, 70, GRAY);
                }
                else {
                    Zeytin::get().play_start_variants();
                    Zeytin::get().play_update_variants();
                    DrawText("PLAY MODE", 1490, 20, 70, BLUE);
                }
            }

#else
                    Zeytin::get().play_start_variants();
                    Zeytin::get().play_update_variants();
#endif

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
