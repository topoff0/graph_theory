#pragma once

// === UI ===
#define UI_HEADER_LENGTH 80
#define UI_MENU_LENGTH 40
#define UI_OPERATION_RESULT_LENGTH UI_MENU_LENGTH
#define UI_LIST_LEFT_PADDING 2
#define UI_TABLE_WIDTH 60
#define UI_HEADER_EXTRA_WIDTH 12
#define UI_MENU_ITEM_EXTRA_WIDTH 10
#define UI_TEXT_HORIZONTAL_PADDING 4
#define UI_PREFORMATTED_LEFT_PADDING 1
#define UI_LINK_LEFT_PADDING 4
#define UI_MESSAGE_EXTRA_WIDTH 8
#define UI_BORDER_WIDTH_ADJUSTMENT 2

// === COLORS ===
#define ANSI_ESCAPE_CHAR '\033'
#define ANSI_ESCAPE_START '['
#define ANSI_SEQUENCE_FINAL_MIN '@'
#define ANSI_SEQUENCE_FINAL_MAX '~'
#define ANSI_RESET_COLOR "\033[0m"
#define ANSI_GREEN_COLOR "\033[32m"
#define ANSI_YELLOW_COLOR "\033[33m"
#define ANSI_BLUE_COLOR "\033[34m"
#define ANSI_RED_COLOR "\033[31m"
#define ANSI_PURPLE_COLOR "\033[35m"
#define ANSI_CYAN_COLOR "\033[36m"

// === TEXT FORMAT ===
#define MAX_LINE_LENGTH 70

// === FILES ===
#define FILES_DIRECTORY_NAME "files"
#define FILES_PARENT_DIRECTORY "../files"

// === WORD OUTPUT ===
#define WORD_OUTPUT_MIN_LIMIT 1
#define WORD_OUTPUT_MAX_LIMIT 100000

// === HASH TABLE ===
#define HASH_INITIAL_CAPACITY 64

// Таблица расширяется, когда unique_words / capacity > 3 / 4.
#define HASH_LOAD_LIMIT_N 3
#define HASH_LOAD_LIMIT_D 4

#define HASH_GROWTH_FACTOR 2

#define HASH_MULTIPLIER 31

// === TREE OUTPUT COLORS ===
// Отдельный красный для узлов дерева, не цвет ошибок.
#define TREE_RED_COLOR "\033[38;5;203m"

// Светло-серый вместо настоящего черного, чтобы было видно в темной консоли.
#define TREE_BLACK_COLOR "\033[38;5;250m"
#define TREE_RESET_COLOR ANSI_RESET_COLOR

// === CONSTANT WORDS ===
#define ERROR_WORD "ОШИБКА"
