#pragma once
#include "io.h"

class app
{
private:
    static const vector<pair<string, COLOR>> main_menu_items;
    static const vector<pair<string, COLOR>> main_menu_exit_items;
    static const vector<pair<string, COLOR>> exit_items;
    static const vector<pair<string, COLOR>> universe_creation_items;
    static const vector<pair<string, COLOR>> multiset_creation_items;
    static const vector<pair<string, COLOR>> operation_items;

    static bool _running;
    static bool _back;

    // === Process commands ===
    static void process_main_menu(const int choice);

    class MainMenu
    {
    public:
        static void start();
    };

public:
    static void run();
};
