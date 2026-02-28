#include "app.h"
#include "menu.h"

const std::vector<std::pair<std::string, COLOR>> app::main_menu_items = {
    {"Начать работу", BASE}};

const std::vector<std::pair<std::string, COLOR>> app::main_menu_exit_items = {
    {"Завершить работу", RED}};

const std::vector<std::pair<std::string, COLOR>> app::exit_items = {
    {"Вернуться в главное меню", RED}, {"Завершить работу", RED}};

bool app::_running = true;
bool app::_back = false;

void app::run() {
  io::print_header("Лабораторная работа №1. Теория графов.", HEADER_STYLE::SIMPLE);
  io::v_space(1);

  Menu main_menu("Главное меню");

  while (_running) {
    _back = false;
    main_menu.show(main_menu_items, main_menu_exit_items, "Список команд");
    int choice =
        io::read_number(-(main_menu_exit_items.size() - 1),
                        main_menu_items.size(), "Введите номер команды");
    process_main_menu(choice);
  }
  io::print_header("Программа завершила свою работу", BOLD, GREEN);
}

void app::process_main_menu(const int choice) {
  switch (choice) {
  case 0:
    _running = false;
    break;
  case 1:
    app::MainMenu::start();
    break;

  default:
    break;
  }
}

void app::MainMenu::start() {}

