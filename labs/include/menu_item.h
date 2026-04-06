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
    {1, "Сгенерировать связный граф", GREEN},
    {2, "Сгенерировать связный ациклический ориентированный граф", GREEN},
    {3, "Сгенерировать весовую матрицу", GREEN},
    {4, "Сгенерировать матрицу пропускных способностей", GREEN},
    {5, "Сгенерировать матрицу стоимости", GREEN},
    {6, "Сделать граф неориентированном деревом", YELLOW},
    {7, "Сделать граф ориентированным", YELLOW},
    {8, "Посчитать эксцентриситеты вершины", BASE},
    {9, "Выделить центральные вершины графа", BASE},
    {10, "Выделить диаметральные вершины графа", BASE},
    {11, "Метод Шимбелла", BASE},
    {12, "Проверить маршруты между вершинами", BASE},
    {13, "Выполнить обход ребер графа поиском в ширину", BASE},
    {14, "Найти кратчайший путь (Беллман-Форд)", BASE},
    {15, "Найти максимальный поток (Форд-Фалкерсон)", BASE},
    {16, "Найти поток минимальной стоимости (2/3 * max)", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> WEIGHT_MODE_MENU = {
    {1, "Только положительные значения", BASE},
    {2, "Только отрицательные значения", BASE},
    {3, "Смешанные значения", BASE}};

static const vector<menu_item> SHIMBELL_MIN_MAX_MENU = {
    {1, "Минимальный путь", BASE}, {2, "Максимальный путь", BASE}};
