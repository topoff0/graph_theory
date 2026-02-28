#pragma once

#include "colors.h"

#include <vector>

using std::pair;
using std::vector;

struct menu_item {
    int id;
    const char *title;
    COLOR clr;
};

inline pair<int, int> menu_min_max_id(const vector<menu_item> &menu) {
    int min_id = menu[0].id;
    int max_id = menu[0].id;

    for (auto &i : menu) {
        if (i.id < min_id)
            min_id = i.id;
        if (i.id > max_id)
            max_id = i.id;
    }

    return {min_id, max_id};
}

static const vector<menu_item> MAIN_MENU = {
    {1, "Начать работу", GREEN},
    {2, "Пртестироврать распределение", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> START_WORK_MENU = {
    {-1, "Вернуться в главное меню", RED}, {0, "Завершить работу", RED}};
