#include "asset_browser/asset_browser.h"
#include "console/console.h"
#include "engine/engine_communication.h"
#include "engine/engine_controls.h"
#include "entity/entity_list.h"
#include "hierarchy/hierarchy.h"
#include "hierarchy/theme.h"
#include "raylib.h"
#include "rlimgui.h"
#include "test_viewer/test_viewer.h"
#include "variant/variant_list.h"
#include "window/window_manager.h"

int main(int argc, char *argv[]) {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT |
                 FLAG_WINDOW_ALWAYS_RUN);

  InitWindow(1600, 900, "QEngine");

  SetTargetFPS(60);
  SetExitKey(0);

  rlImGuiSetup(true);
  SetEditorTheme();

  EngineControls engine_controls;
  EngineCommunication engine_communication;

  EntityList entity_list;
  VariantList variant_list;

  Hierarchy hierarchy(entity_list.get_entities(), variant_list.get_variants());
  TestViewer test_viewer;

  WindowManager window_manager;
  window_manager.init();

  window_manager.add_window(
      "Hierarchy", [&hierarchy]() { hierarchy.update(); }, true, "Hierarchy",
      true);

  window_manager.add_window(
      "Console", []() { ConsoleWindow::get().render(); }, true, "Console",
      true);

  window_manager.add_window(
      "Asset Browser", []() { AssetBrowser::get().render(); }, true,
      "Asset Browser", true);

  window_manager.add_window(
      "Test Viewer", [&test_viewer]() { test_viewer.render(); }, false,
      "Test Viewer", true);

  window_manager.add_main_menu_component(
      [&engine_controls] { engine_controls.render(); });

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    rlImGuiBegin();

    window_manager.render();

    rlImGuiEnd();
    EndDrawing();
  }

  engine_controls.kill_engine();

  rlImGuiShutdown();
  CloseWindow();

  return 0;
}
