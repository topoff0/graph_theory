#include "menu_func.h"
#include "colors.h"
#include "config.h"
#include "distribution.h"
#include "graph.h"
#include "io.h"
#include "matrix.h"
#include "menu_item.h"
#include <cstdlib>
#include <ctime>
#include <string>

static graph *current_graph = nullptr;
static matrix *current_matrix = nullptr;

void menu_func::back_to_main_menu(bool &back) { back = true; }

void menu_func::MainMenu::test_distribution() {
    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> values = dist.sample(1000);
    dist.print_histogram(values, 20, COLOR::GREEN, 60);
}

void menu_func::StartWorkMenu::generate_graph() {
    size_t n;
    n = io::read_number({2, MAX_VERTICES_COUNT}, "Введите количество вершин");

    if (current_graph != nullptr)
        delete current_graph;

    current_graph = new graph(n);

    current_graph->generate_connected_graph();

    io::print_header("Сгенерирован граф", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::generate_acyclic_oriented_graph() {
    size_t n;
    n = io::read_number({2, MAX_VERTICES_COUNT}, "Введите количество вершин");

    if (current_graph != nullptr)
        delete current_graph;

    current_graph = new graph(n);

    current_graph->generate_DAG();

    io::print_header("Сгенерирован граф", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::generate_weights_matrix() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    io::print_command_menu(WEIGHT_MODE_MENU, "Выбирете вариант заполнения");
    int mode_number = io::read_number(menu_min_max_id(WEIGHT_MODE_MENU),
                                      "Введите номер варианта");

    current_graph->generate_weight_matrix(static_cast<WeightMode>(mode_number));
    io::print_header("Сгенерирована весовая матрица", BOLD);
    io::print_matrix(current_graph->get_weights(), "Весовая матрица", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_acyclic() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    if (current_graph->has_status(ACYCLIC)) {
        io::print_error("Граф уже ациклический");
        io::wait_enter();
        return;
    }

    current_graph->make_graph_acyclic_not_oriented();

    io::print_header("Граф скорректирован: ациклический", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_oriented() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    if (current_graph->has_status(ORIENTED)) {
        io::print_error("Граф уже ориентированный");
        io::wait_enter();
        return;
    }

    current_graph->make_graph_oriented();

    io::print_header("Граф скорректирован: ориентированный", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::calc_eccentricities() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    int index = io::read_number({0, current_graph->get_size() - 1},
                                "Введите индекс вершины");

    vector<int> ecc = current_graph->calc_ecc(index);

    string text = "[ ";
    for (int i = 0; i < ecc.size(); i++) {
        text.append(std::to_string(ecc[i]));
        if (i != ecc.size() - 1)
            text.append(", ");
    }
    text.append(" ]");

    io::print_text_with_header(text, "Эксцентрисететы графа", "", BOXED, GREEN);
    io::wait_enter();
}
void menu_func::StartWorkMenu::calc_centers() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    vector<char> centers = current_graph->calc_central_vertices();
    string text = "[ ";
    for (int i = 0; i < centers.size(); i++) {
        text += centers[i];
        if (i != centers.size() - 1)
            text.append(" ");
    }
    text.append(" ]");

#if DEBUG
    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif

    io::print_text_with_header(text, "Центры графа", "", BOXED, GREEN);
    io::wait_enter();
}
void menu_func::StartWorkMenu::calc_diameter() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    vector<char> diameters = current_graph->calc_diametral_vertices();
    string text = "[ ";
    for (int i = 0; i < diameters.size(); i++) {
        text += diameters[i];
        if (i != diameters.size() - 1)
            text.append(" ");
    }
    text.append(" ]");

#if DEBUG
    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif

    io::print_text_with_header(text, "Диаметральные вершины графа", "", BOXED,
                               GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::run_shimbell_method() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    if (current_graph->is_weight_mode(EMPTY)) {
        io::print_error("Сначала сгенерируйте весовую матрицу");
        io::wait_enter();
        return;
    }

    int edges =
        io::read_number({1, current_graph->get_size()}, "Введите число ребер");
    io::print_command_menu(SHIMBELL_MIN_MAX_MENU, "Выбирите вариант пути");
    int mode_number = io::read_number(menu_min_max_id(SHIMBELL_MIN_MAX_MENU),
                                      "Введите номер варианта");
    bool find_max = (mode_number == 2);

    matrix result = current_graph->run_shimbell(edges, find_max);
#if DEBUG
    io::print_matrix(current_graph->get_weights(),
                     "DEBUG: Весовая матрица для проверки", YELLOW);

#endif
    string matrix_header = find_max ? "Матрица максимальных расстояний"
                                    : "Матрица минимальных расстояний";
    io::print_matrix(result, matrix_header, CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::check_routes() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    if (!current_graph->has_status(ORIENTED) ||
        !current_graph->has_status(ACYCLIC)) {
        io::print_error("Для подсчета маршрутов граф должен быть "
                        "ориентированным и ациклическим");
        io::wait_enter();
        return;
    }

    int start = io::read_number({0, current_graph->get_size() - 1},
                                "Введите индекс начальной вершины");
    int end = io::read_number({0, current_graph->get_size() - 1},
                              "Введите индекс конечной вершины");

    unsigned long long routes = current_graph->count_routes(
        static_cast<size_t>(start), static_cast<size_t>(end));

    string text;
    if (routes == 0) {
        text = "Маршруты между заданными вершинами отсутствуют";
    } else {
        text = "Количество маршрутов: " + std::to_string(routes);
    }

    io::print_text_with_header(text, "Результат проверки маршрутов", "", BOXED,
                               GREEN);
    io::wait_enter();
}
