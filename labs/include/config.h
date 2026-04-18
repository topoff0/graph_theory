#pragma once

// === UI ===
#define UI_HEADER_LENGTH 80
#define UI_MENU_LENGTH 40
#define UI_OPERATION_RESULT_LENGTH UI_MENU_LENGTH
#define UI_LIST_LEFT_PADDING 2
#define UI_TABLE_WIDTH 60

// === TEXT FORMAT ===
#define MAX_LINE_LENGTH 70

// === DISTRIBUTION PARAMETERS ===
// #define MU 4 // центр распределения
// #define ALPHA 1.57 // масштаб (крутизна) распределения

#define MU 2.25   // центр распределения
#define ALPHA 2.0 // масштаб (крутизна) распределения

#define WEIGHT_MU 0.0    // центр распределения
#define WEIGHT_ALPHA 0.5 // масштаб (крутизна) распределения

#define THROUGHPUT_MU 5.0    // центр распределения пропускных способностей
#define THROUGHPUT_ALPHA 1.5 // масштаб распределения пропускных способностей

#define COST_MU 3.0    // центр распределения стоимости
#define COST_ALPHA 1.8 // масштаб распределения стоимости

#define MAX_VERTICES_COUNT 20

// === CONSTANT WORDS ===
#define ERROR_WORD "ОШИБКА"

#define DEBUG true
