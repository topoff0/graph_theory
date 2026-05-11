#pragma once

// === UI ===
#define UI_HEADER_LENGTH 80
#define UI_MENU_LENGTH 40
#define UI_OPERATION_RESULT_LENGTH UI_MENU_LENGTH
#define UI_LIST_LEFT_PADDING 2
#define UI_TABLE_WIDTH 60

// === TEXT FORMAT ===
#define MAX_LINE_LENGTH 70

// === HASH TABLE ===
// Начальный размер массива бакетов.
#define HASH_INITIAL_CAPACITY 64

// Таблица расширяется, когда unique_words / capacity > 3 / 4.
#define HASH_LOAD_LIMIT_NUMERATOR 3
#define HASH_LOAD_LIMIT_DENOMINATOR 4

// При расширении размер таблицы умножается на 2.
#define HASH_GROWTH_FACTOR 2

// Множитель для простой полиномиальной хеш-функции:
// hash = hash * HASH_MULTIPLIER + очередной_байт.
#define HASH_MULTIPLIER 31

// === TREE OUTPUT COLORS ===
// Отдельный красный для узлов дерева, не цвет ошибок.
#define TREE_RED_COLOR "\033[38;5;203m"

// Светло-серый вместо настоящего черного, чтобы было видно в темной консоли.
#define TREE_BLACK_COLOR "\033[38;5;250m"
#define TREE_RESET_COLOR "\033[0m"

// === CONSTANT WORDS ===
#define ERROR_WORD "ОШИБКА"
