#pragma once

class app {
  public:
    static void run();

  private:
    static bool _running;
    static bool _back;

    static void start();

    static void handle_main_menu(int choice);
    static void handle_start_work_menu(int choice);
};
