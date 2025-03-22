#include <raylib.h> // IWYU pragme: keep
#include <iostream> // IWYU pragma: keep

#include "core/zeytin.h" // IWYU pragma: keep
#include "editor/editor_communication.h" // IWYU pragma: keep
#include "editor/editor_event.h"
#include "remote_logger/remote_logger.h"

#include "game/rttr_registration.h"

int main(int argc, char* argv[]) {

#if EDITOR_MODE
    EditorCommunication editor_comm;

    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST |  FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_ALWAYS_RUN );
    const int windowWidth = 1280;
    const int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "ZeytinEngine"); 
    SetWindowPosition(466, 172);

    Zeytin::get().generate_variants();
    Zeytin::get().subscribe_editor_events();
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

    bool found_camera = false;
    for (const auto& [entity_id, variants] : Zeytin::get().get_storage()) {
        for (const auto& variant : variants) {
            if (variant.get_type() == rttr::type::get<Camera2DSystem>()) {
                found_camera = true;
                break;
            }
        }
        if (found_camera) {
            log_info() << "Camera system is found" << std::endl;
            break;
        }
    }

    if (!found_camera) {
        entity_id camera_entity = Zeytin::get().new_entity_id();
        VariantCreateInfo info;
        info.entity_id = camera_entity;
        Zeytin::get().add_variant(camera_entity, Camera2DSystem(info));
        
        log_warning() << "Created default camera system" << std::endl;
    }

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
            
            Camera2D* active_camera = nullptr;
            for (auto& [entity_id, variants] : Zeytin::get().get_storage()) {
                for (auto& variant : variants) {
                    if (variant.get_type() == rttr::type::get<Camera2DSystem>()) {
                        Camera2DSystem& camera_system = variant.get_value<Camera2DSystem&>();
                        active_camera = &camera_system.get_camera();
                        break;
                    }
                }
                if (active_camera) break;
            }
            
            if (active_camera) {
                BeginMode2D(*active_camera);
            }
            
            Zeytin::get().post_init_variants();
            Zeytin::get().update_variants();
            
            if (active_camera) {
                EndMode2D();
            }
            
            DrawText("1920x1080", 20, 20, 40, BLACK);
            
        #ifdef EDITOR_MODE
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
