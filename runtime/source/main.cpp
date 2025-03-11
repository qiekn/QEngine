#include <raylib.h> // IWYU pragme: keep
#include <iostream> // IWYU pragma: keep

#include "core/zeytin.h" // IWYU pragma: keep
#include "editor/editor_communication.h" // IWYU pragma: keep
#include "game/rttr_registration.h"


int main(int argc, char* argv[]) {

#ifdef EDITOR_MODE 
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST);
#endif

    bool kill_after_generate = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--kill-after-generate") == 0) {
            kill_after_generate = true;
            break;
        }
    }

    Zeytin::get().generate_variants();

    if(kill_after_generate) {
        std::cout << "Kill After Generate" << std::endl;
        exit(0);
    }

    const int virtualWidth = 1920;
    const int virtualHeight = 1080;

    const int windowWidth = 800;
    const int windowHeight = 600;

    InitWindow(windowWidth, windowHeight, "Game - ZeytinEngine");
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

#ifdef EDITOR_MODE
        editor_comm.heartbeet();
        editor_comm.raise_events();
#endif

        Vector2 mousePosition = GetMousePosition();

        Vector2 virtualMousePosition = {
            (mousePosition.x - position.x) / scale,
            (mousePosition.y - position.y) / scale
        };

        BeginTextureMode(target);
            ClearBackground(RAYWHITE);

            Zeytin::get().update_variants(); // this should be here because variants may render something

#ifdef EDITOR_MODE
            DrawText("1920x1080", 20, 20, 40, BLACK);
            DrawCircle(virtualMousePosition.x, virtualMousePosition.y, 10, GREEN);
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
