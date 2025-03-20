#include <raylib.h> // IWYU pragme: keep
#include <iostream> // IWYU pragma: keep
#include "core/zeytin.h" // IWYU pragma: keep
#include "editor/editor_communication.h" // IWYU pragma: keep
#include "editor/editor_event.h"
#include "game/rttr_registration.h"

int main(int argc, char* argv[]) {

#if EDITOR_MODE
    EditorCommunication editor_comm;
    Zeytin::get().generate_variants();
    Zeytin::get().subscribe_editor_events();
#endif

#ifdef EDITOR_MODE 
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST |  FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_ALWAYS_RUN );
    const int windowWidth = 1280;
    const int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "ZeytinEngine"); 
    SetWindowPosition(466, 172);

    EditorEventBus::get().subscribe<const rapidjson::Document&>(
       EditorEvent::WindowStateChanged,
       [](const rapidjson::Document& doc) {
           if (doc.HasMember("is_minimize")) {
               bool is_minimize = doc["is_minimize"].GetBool();

               if(is_minimize) {
                    MinimizeWindow();
               }
   
           }
       }
   );

#else
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    InitWindow(windowWidth, windowHeight, "Game");
#endif

    const int virtualWidth = 1920;
    const int virtualHeight = 1080;
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

#ifndef EDITOR_MODE
    Zeytin::get().deserialize_entities();
#endif
    while (!WindowShouldClose() 
            #ifdef EDITOR_MODE
             &&   !Zeytin::get().should_die()
            #endif
          ) {


#ifdef EDITOR_MODE
            editor_comm.raise_events();
            if(!Zeytin::get().is_scene_ready())  {
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
                continue;
            }
#endif
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
#ifdef EDITOR_MODE
            Zeytin::get().post_init_variants();
            Zeytin::get().update_variants(); 
            DrawText("1920x1080", 20, 20, 40, BLACK);

            if(IsKeyPressed(KEY_H)) {
                if(IsWindowMinimized()) {
                    SetWindowFocused();
                }
                else {
                    MinimizeWindow();
                }
            }

            // NOTE: temp solution, to notify editor with updated variants
            if(!Zeytin::get().m_synced_once) {
                Zeytin::get().sync_editor(); 
                Zeytin::get().m_synced_once = true;
            }

            if(Zeytin::get().is_play_mode()) {
                if(Zeytin::get().is_paused_play_mode()) {
                    DrawText("PAUSED", 1610, 20, 70, GRAY);
                }
                else {
                    Zeytin::get().play_start_variants();
                    Zeytin::get().play_update_variants();
                    DrawText("PLAY MODE", 1490, 20, 70, BLUE);
                    Zeytin::get().sync_editor(); 
                }
            }
#else
                    Zeytin::get().post_init_variants();
                    Zeytin::get().update_variants(); 
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
