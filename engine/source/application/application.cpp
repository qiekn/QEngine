#include "application/application.h"
#include "core/raylib_wrapper.h"
#include "core/zeytin.h"
#include "raylib.h"
#include "core/macros.h""

Application::Application() {
    init_window();
    init_engine();
}

void Application::init_window() {

#ifdef EDITOR_MODE
    SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TOPMOST | FLAG_WINDOW_ALWAYS_RUN);

    const int windowWidth = 1280;
    const int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Engine View");
#else
    const int windowWidth = GetScreenWidth();
    const int windowHeight = GetScreenHeight();
    InitWindow(windowWidth, windowHeight, "Zeytin Game");
#endif

    int monitor_refresh_rate = GetMonitorRefreshRate(GetCurrentMonitor());
    set_target_fps(monitor_refresh_rate);
    set_exit_key(0);
}

void Application::init_engine() {
    CONSTRUCT_SINGLETON(Zeytin);
}

void Application::run_frame() {
    Zeytin::get().run_frame();
}

bool Application::should_shutdown() {
    return Zeytin::get().should_die();
}

void Application::shutdown() {

}
