#pragma once
#include "io.h"

struct Menu {
private:
  string header;

public:
  Menu(const string &h) : header(h) {}

  void show(const vector<pair<string, COLOR>> menu_items,
            const vector<pair<string, COLOR>> exit_items,
            const string &list_header, HEADER_STYLE style = BOLD);
};
