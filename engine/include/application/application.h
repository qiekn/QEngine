#pragma once

#include <raylib.h>
#include <string>

class Application {
public:
    Application();

    void run();
    void shutdown();
    bool should_shutdown();

private:
    void init_window();
    void init_engine();
};
