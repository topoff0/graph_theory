#include "menu_.h"

void menu::show(const vector<menu_item> &menu_items, const string &list_header,
                HEADER_STYLE style) {
    io::print_header(header, HEADER_STYLE::BOLD);
    io::print_command_menu(menu_items, list_header);
}
