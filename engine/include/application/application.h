#pragma once

class Application {
public:
  Application();

  void run_frame();
  void shutdown();
  bool should_shutdown();

private:
  void init_window();
  void init_engine();
};
