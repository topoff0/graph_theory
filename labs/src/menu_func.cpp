#include "menu_func.h"
#include "colors.h"
#include "config.h"
#include "distribution.h"
#include "graph.h"
#include "io.h"
#include "matrix.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

static graph *current_graph = nullptr;
static matrix *current_matrix = nullptr;

void menu_func::back_to_main_menu(bool &back) { back = true; }

void menu_func::MainMenu::test_distribution() {
    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    auto values = dist.sample(1000);
    dist.print_histogram(values, 20, COLOR::GREEN, 60);
}

void menu_func::StartWorkMenu::generate_graph() {
    size_t n;
    n = io::read_number({2, 15}, "Введите количество вершин");

    if (current_graph != nullptr)
        delete current_graph;
    current_graph = new graph(n);

    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    auto deg_values = dist.sample(n);

    vector<int> degrees(n, 0);
    for (size_t i = 0; i < n; i++) {
        degrees[i] = static_cast<int>(std::round(deg_values[i]));
    }

    current_graph->generate(degrees);

    io::print_header("Сгенерирован граф", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_acyclic() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }

    if (current_graph->has_status(ACYCLIC)) {
        io::print_error("Граф уже ациклический");
        io::wait_enter();
        return;
    }

    current_graph->make_graph_acyclic();

    io::print_header("Граф скорректирован: ациклический", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_oriented() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
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
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }
    int index = io::read_number({0, current_graph->get_size() - 1}, "Введите индекс вершины");

    vector<int> ecc = current_graph->calc_ecc(index);

    string after = "[ ";
    for (int i = 0; i < ecc.size(); i++) {
        after.append(std::to_string(ecc[i]));
        if (i != ecc.size() - 1)
            after.append(", ");
    }
    after.append(" ]");

    io::print_text_with_header(after, "Эксцентрисететы графа", "", BOXED,
                               GREEN);
}
void menu_func::StartWorkMenu::calc_centers() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }
    // TODO: Сделать расчет центров
}
void menu_func::StartWorkMenu::calc_diameter() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }
    // TODO: Сделать расчет диметральных вершин
}

void menu_func::StartWorkMenu::run_shimbell() {}

void menu_func::StartWorkMenu::check_routes() {}
