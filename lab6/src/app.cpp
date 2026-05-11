#include "app.h"
#include "colors.h"
#include "io.h"
#include "menu_.h"
#include "menu_item.h"
#include "text_utils.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <system_error>
#include <vector>

using namespace std;

bool app::_running = true;
bool app::_back = false;
RedBlackTreeDictionary app::_tree_dictionary;
HashDictionary app::_hash_dictionary;

namespace {

namespace fs = filesystem;

string read_word() {
    string word = io::read_string("Введите слово");
    word = text_utils::normalize_word(word);
    return word;
}

void print_word_error() { io::print_error("Слово не содержит букв"); }

void print_info(const string &header, const string &message) {
    io::print_text_with_header(message, header, "", BOXED, GREEN);
}

string find_files_directory() {
    const vector<string> candidates = {"files", "../files"};

    for (const string &candidate : candidates) {
        error_code error;
        if (fs::exists(candidate, error) && fs::is_directory(candidate, error))
            return candidate;
    }

    return "";
}

vector<fs::path> collect_files(const string &directory) {
    vector<fs::path> files;
    error_code error;

    for (const auto &entry : fs::directory_iterator(directory, error)) {
        if (error)
            break;

        error_code file_error;
        if (entry.is_regular_file(file_error))
            files.push_back(entry.path());
    }

    sort(files.begin(), files.end(),
              [](const fs::path &left, const fs::path &right) {
                  return left.filename().string() < right.filename().string();
              });

    return files;
}

string read_file_path() {
    io::print_command_menu(FILE_SOURCE_MENU, "Выбор файла");
    int source = io::read_number(menu_min_max_id(FILE_SOURCE_MENU),
                                 "Введите номер варианта");

    switch (source) {
    case 1:
        break;
    case 2:
        return io::read_string("Введите путь к текстовому файлу");
    default:
        io::print_error("Неизвестная команда");
        return io::read_string("Введите путь к текстовому файлу");
    }

    string directory = find_files_directory();
    if (directory.empty()) {
        io::print_error("Директория files не найдена");
        return io::read_string("Введите путь к текстовому файлу");
    }

    vector<fs::path> files = collect_files(directory);
    if (files.empty()) {
        io::print_error("В директории files нет файлов");
        return io::read_string("Введите путь к текстовому файлу");
    }

    ostringstream list;
    for (size_t i = 0; i < files.size(); ++i) {
        list << i + 1 << ". " << files[i].filename().string() << "\n";
    }
    list << "0. Ввести путь вручную";

    io::print_preformatted_with_header(list.str(),
                                       "Файлы директории " + directory, CYAN);

    int choice = io::read_number({0, static_cast<int>(files.size())},
                                 "Выберите номер файла");
    if (choice == 0)
        return io::read_string("Введите путь к текстовому файлу");

    return files[static_cast<size_t>(choice - 1)].string();
}

void print_tree_stats(const RedBlackTreeDictionary &dict) {
    ostringstream text;
    text << "Уникальных слов: " << dict.unique_words() << "\n"
         << "Всего добавлений слов: " << dict.total_words() << "\n"
         << "Высота дерева: " << dict.height();
    io::print_text_with_header(text.str(), "Красно-черное дерево", "", BOXED,
                               CYAN);
}

void print_hash_stats(const HashDictionary &dict) {
    ostringstream text;
    text << "Уникальных слов: " << dict.unique_words() << "\n"
         << "Всего добавлений слов: " << dict.total_words() << "\n"
         << "Размер таблицы: " << dict.capacity() << "\n"
         << "Бакетов с коллизиями: " << dict.collision_buckets() << "\n"
         << "Максимальная длина цепочки: " << dict.max_chain_length();
    io::print_text_with_header(text.str(), "Хеш-таблица", "", BOXED, CYAN);
}

} // namespace

void app::run() {
    io::print_header("Лабораторная работа №6. Словари", SIMPLE, CYAN);
    io::v_space(1);

    menu main_menu("Главное меню");

    while (_running) {
        _back = false;
        main_menu.show(MAIN_MENU, "Список команд");
        int choice = io::read_number(menu_min_max_id(MAIN_MENU),
                                     "Введите номер команды");
        handle_main_menu(choice);
    }

    io::print_header("Программа завершила свою работу", BOLD, GREEN);
}

void app::handle_main_menu(int choice) {
    switch (choice) {
    case 0:
        _running = false;
        break;
    case 1:
        work_with_tree_dictionary();
        break;
    case 2:
        work_with_hash_dictionary();
        break;
    case 3:
        load_file_to_both();
        break;
    case 4:
        show_all_stats();
        io::wait_enter();
        break;
    case 5:
        clear_both();
        break;
    default:
        io::print_error("Неизвестная команда");
        break;
    }
}

void app::work_with_tree_dictionary() {
    menu dictionary_menu("Словарь на красно-черном дереве");
    while (_running && !_back) {
        dictionary_menu.show(TREE_DICTIONARY_MENU, "Список команд");
        int choice = io::read_number(menu_min_max_id(TREE_DICTIONARY_MENU),
                                     "Введите номер команды");
        handle_tree_menu(choice);
    }
}

void app::work_with_hash_dictionary() {
    menu dictionary_menu("Словарь на хеш-таблице");
    while (_running && !_back) {
        dictionary_menu.show(HASH_DICTIONARY_MENU, "Список команд");
        int choice = io::read_number(menu_min_max_id(HASH_DICTIONARY_MENU),
                                     "Введите номер команды");
        handle_hash_menu(choice);
    }
}

void app::handle_tree_menu(int choice) {
    switch (choice) {
    case -1:
        _back = true;
        return;
    case 0:
        _running = false;
        return;
    case 1: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else {
            bool inserted = _tree_dictionary.insert(word);
            print_info("Добавление",
                       inserted ? "Слово добавлено" : "Счетчик слова увеличен");
        }
        break;
    }
    case 2: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else if (_tree_dictionary.erase(word)) {
            print_info("Удаление", "Слово удалено");
        } else {
            io::print_error("Слова нет в словаре");
        }
        break;
    }
    case 3: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else if (_tree_dictionary.contains(word)) {
            print_info("Поиск",
                       "Слово найдено. Количество: " +
                           to_string(_tree_dictionary.count(word)));
        } else {
            io::print_error("Слово не найдено");
        }
        break;
    }
    case 4: {
        string path = read_file_path();
        string error;
        size_t loaded = _tree_dictionary.load_from_file(path, error);
        if (!error.empty())
            io::print_error(error);
        else
            print_info("Загрузка", "Загружено слов: " + to_string(loaded));
        break;
    }
    case 5:
        print_tree_stats(_tree_dictionary);
        break;
    case 6: {
        int limit = io::read_number({1, 100000}, "Сколько слов вывести");
        ostringstream text;
        _tree_dictionary.print(text, limit);
        io::print_preformatted_with_header(text.str(), "Слова дерева", CYAN);
        break;
    }
    case 7:
        _tree_dictionary.clear();
        print_info("Очистка", "Словарь очищен");
        break;
    case 8: {
        ostringstream text;
        _tree_dictionary.print_tree(text);
        io::print_preformatted_with_header(text.str(), "Красно-черное дерево",
                                           CYAN);
        break;
    }
    default:
        io::print_error("Неизвестная команда");
        break;
    }

    io::wait_enter();
}

void app::handle_hash_menu(int choice) {
    switch (choice) {
    case -1:
        _back = true;
        return;
    case 0:
        _running = false;
        return;
    case 1: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else {
            bool inserted = _hash_dictionary.insert(word);
            print_info("Добавление",
                       inserted ? "Слово добавлено" : "Счетчик слова увеличен");
        }
        break;
    }
    case 2: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else if (_hash_dictionary.erase(word)) {
            print_info("Удаление", "Слово удалено");
        } else {
            io::print_error("Слова нет в словаре");
        }
        break;
    }
    case 3: {
        string word = read_word();
        if (word.empty()) {
            print_word_error();
        } else if (_hash_dictionary.contains(word)) {
            print_info("Поиск",
                       "Слово найдено. Количество: " +
                           to_string(_hash_dictionary.count(word)));
        } else {
            io::print_error("Слово не найдено");
        }
        break;
    }
    case 4: {
        string path = read_file_path();
        string error;
        size_t loaded = _hash_dictionary.load_from_file(path, error);
        if (!error.empty())
            io::print_error(error);
        else
            print_info("Загрузка", "Загружено слов: " + to_string(loaded));
        break;
    }
    case 5:
        print_hash_stats(_hash_dictionary);
        break;
    case 6: {
        int limit = io::read_number({1, 100000}, "Сколько слов вывести");
        ostringstream text;
        _hash_dictionary.print(text, limit);
        io::print_preformatted_with_header(text.str(), "Слова хеш-таблицы",
                                           CYAN);
        break;
    }
    case 7:
        _hash_dictionary.clear();
        print_info("Очистка", "Словарь очищен");
        break;
    case 8: {
        ostringstream text;
        _hash_dictionary.print_table(text);
        io::print_preformatted_with_header(text.str(), "Хеш-таблица", CYAN);
        break;
    }
    default:
        io::print_error("Неизвестная команда");
        break;
    }

    io::wait_enter();
}

void app::load_file_to_both() {
    string path = read_file_path();

    string tree_error;
    size_t tree_loaded = _tree_dictionary.load_from_file(path, tree_error);
    if (!tree_error.empty()) {
        io::print_error(tree_error);
        io::wait_enter();
        return;
    }

    string hash_error;
    size_t hash_loaded = _hash_dictionary.load_from_file(path, hash_error);
    if (!hash_error.empty()) {
        io::print_error(hash_error);
        io::wait_enter();
        return;
    }

    print_info("Загрузка", "В красно-черное дерево загружено слов: " +
                               to_string(tree_loaded) +
                               "\nВ хеш-таблицу загружено слов: " +
                               to_string(hash_loaded));
    io::wait_enter();
}

void app::clear_both() {
    _tree_dictionary.clear();
    _hash_dictionary.clear();
    print_info("Очистка", "Оба словаря полностью очищены");
    io::wait_enter();
}

void app::show_all_stats() {
    print_tree_stats(_tree_dictionary);
    print_hash_stats(_hash_dictionary);
}
