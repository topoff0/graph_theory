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
        menu start_work_menu("Работа с графами");
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
        menu_func::StartWorkMenu::generate_acyclic_oriented_graph();
        break;
    case 3:
        menu_func::StartWorkMenu::generate_weights_matrix();
        break;
    case 4:
        menu_func::StartWorkMenu::make_graph_tree();
        break;
    case 5:
        menu_func::StartWorkMenu::make_graph_oriented();
        break;
    case 6:
        menu_func::StartWorkMenu::calc_eccentricities();
        break;
    case 7:
        menu_func::StartWorkMenu::calc_centers();
        break;
    case 8:
        menu_func::StartWorkMenu::calc_diameter();
        break;
    case 9:
        menu_func::StartWorkMenu::run_shimbell_method();
    case 10:
        menu_func::StartWorkMenu::check_routes();

    default:
        break;
    }
}
