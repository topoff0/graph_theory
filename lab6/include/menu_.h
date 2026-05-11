#pragma once
#include "io.h"
#include "menu_item.h"

struct menu {
  private:
    string header;

  public:
    menu(const string &h) : header(h) {}

    void show(const vector<menu_item> &menu_items, const string &list_header,
              HEADER_STYLE style = BOLD);
};
