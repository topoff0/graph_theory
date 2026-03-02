#include "app.h"
#include "colors.h"
#include "menu_.h"
#include "menu_func.h"

bool app::_running = true;
bool app::_back = false;

void app::run() {
    io::print_header("Лабораторная работа №1. Теория графов.",
                     HEADER_STYLE::SIMPLE, CYAN);
    io::v_space(1);

    menu main_menu("Главное меню");

    while (_running) {
        _back = false;
        main_menu.show(MAIN_MENU, "Список команд");

        int choice = io::read_number(menu_min_max_id(MAIN_MENU),
                                     "Введите номер команды");
        handle_main_menu(choice);
    }
    io::print_header("Программа завершила свою работу", BOLD, GREEN);
}

void app::handle_main_menu(int choice) {
    switch (choice) {
    case 0:
        _running = false;
        break;
    case 1:
        start();
        break;
    case 2:
        menu_func::MainMenu::test_distribution();

    default:
        break;
    }
}

void app::start() {
    while (_running && !_back) {
        menu start_work_menu("Стартовое меню");
        start_work_menu.show(START_WORK_MENU, "Список команд");

        int choice = io::read_number(menu_min_max_id(START_WORK_MENU),
                                     "Введите номер команды");
        handle_start_work_menu(choice);
    }
}

void app::handle_start_work_menu(int choice) {
    switch (choice) {
    case -1:
        menu_func::back_to_main_menu(_back);
        break;
    case 0:
        _running = false;
        break;
    case 1:
        menu_func::StartWorkMenu::generate_graph();
        break;
    case 2:
        menu_func::StartWorkMenu::calc_eccentricities();
        break;
    case 3:
        menu_func::StartWorkMenu::calc_centers();
        break;
    case 4:
        menu_func::StartWorkMenu::calc_diameter();
        break;

    default:
        break;
    }
}
