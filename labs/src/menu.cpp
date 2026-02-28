#include "menu.h"

void Menu::show(const vector<pair<string, COLOR>> menu_items, const vector<pair<string, COLOR>> exit_items, const string &list_header, HEADER_STYLE style)
{
    io::print_header(header, HEADER_STYLE::BOLD);
    io::print_command_menu(menu_items, exit_items, list_header);
}
