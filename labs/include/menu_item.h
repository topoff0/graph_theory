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
    {1, "Сгенерировать связный граф", BASE},
    {2, "Сгенерировать весовую матрицу", BASE},
    {3, "Сделать граф ациклическим", BASE},
    {4, "Сделать граф ориентированным", BASE},
    {5, "Посчитать эксцентриситеты вершины", BASE},
    {6, "Выделить центральные вершины графа", BASE},
    {7, "Выделить диаметральные вершины графа", BASE},
    {8, "Метод Шимбелла", BASE},
    {9, "Проверить маршруты между вершинами", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> WEIGHT_MODE_MENU = {
    {1, "Только положительные значения", BASE},
    {2, "Только отрицательные значения", BASE},
    {3, "Смешанные значения", BASE}};

static const vector<menu_item> SHIMBELL_MIN_MAX_MENU = {
    {1, "Минимальный путь", BASE},
    {2, "Максимальный путь", BASE}};
