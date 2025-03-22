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

    SetWindowPosition(466, 172);
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
