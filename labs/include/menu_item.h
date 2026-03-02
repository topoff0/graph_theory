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

    for (size_t i = 0; i < menu.size(); ++i) {
        if (menu[i].id < min_id)
            min_id = menu[i].id;
        if (menu[i].id > max_id)
            max_id = menu[i].id;
    }

    return {min_id, max_id};
}

static const vector<menu_item> MAIN_MENU = {
    {1, "Начать работу", GREEN},
    {2, "Пртестироврать распределение", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> START_WORK_MENU = {
    {-1, "Вернуться в главное меню", RED},
    {1, "Сгенерировать связный ациклический граф", BASE},
    {2, "Посчитать эксцентриситеты", BASE},
    {3, "Посчитать центр графа", BASE},
    {4, "Посчитать диаметр", BASE},
    {0, "Завершить работу", RED}};
;
