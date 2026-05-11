#pragma once

#include "colors.h"

#include <vector>

using namespace std;

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
    {1, "Работать со словарем на красно-черном дереве", GREEN},
    {2, "Работать со словарем на хеш-таблице", GREEN},
    {3, "Загрузить текстовый файл в оба словаря", BLUE},
    {4, "Показать статистику словарей", BASE},
    {5, "Полностью очистить оба словаря", YELLOW},
    {0, "Завершить работу", RED}};

static const vector<menu_item> TREE_DICTIONARY_MENU = {
    {-1, "Вернуться в главное меню", RED},
    {1, "Добавить слово", GREEN},
    {2, "Удалить слово", YELLOW},
    {3, "Найти слово", BASE},
    {4, "Загрузить/дополнить словарь из текстового файла", BLUE},
    {5, "Показать статистику", BASE},
    {6, "Вывести первые N слов", BASE},
    {7, "Полностью очистить словарь", YELLOW},
    {8, "Вывести красно-черное дерево", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> HASH_DICTIONARY_MENU = {
    {-1, "Вернуться в главное меню", RED},
    {1, "Добавить слово", GREEN},
    {2, "Удалить слово", YELLOW},
    {3, "Найти слово", BASE},
    {4, "Загрузить/дополнить словарь из текстового файла", BLUE},
    {5, "Показать статистику", BASE},
    {6, "Вывести первые N слов", BASE},
    {7, "Полностью очистить словарь", YELLOW},
    {8, "Вывести хеш-таблицу", BASE},
    {0, "Завершить работу", RED}};

static const vector<menu_item> FILE_SOURCE_MENU = {
    {1, "Выбрать файл из директории files", GREEN},
    {2, "Ввести путь к файлу вручную", BASE}};
