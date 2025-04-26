#include "application/application.h"
#include "core/zeytin.h"

Application::Application() {
    init_window();
    init_engine();
}

void Application::init_window() {

#ifdef EDITOR_MODE
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_ALWAYS_RUN);

    const int windowWidth = 1280;
    const int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "ZeytinEngine");

    float console_height = GetScreenHeight() * 0.35; // magical aspect, from LayoutConfig editor
    float free_height = GetScreenHeight() - console_height;
    float window_start_height = free_height/3;

    float hierarchy_width = GetScreenWidth() * 0.18; // magical aspect, from LayoutConfig editor
    float asset_browser_width = GetScreenWidth() * 0.315; // magical aspect, from LayoutConfig editor
    float free_width = GetScreenWidth() - hierarchy_width - asset_browser_width;


    //SetWindowPosition(466, 172);
    SetWindowPosition(hierarchy_width*2+5, window_start_height);
#else
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    InitWindow(windowWidth, windowHeight, "Zeytin Game");
#endif

    SetExitKey(0);
}

void Application::init_engine() {
    get_zeytin().init();
}

void Application::run() {
    get_zeytin().run_frame();
}

bool Application::should_shutdown() {
    return get_zeytin().should_die();
}

void Application::shutdown() {

}
