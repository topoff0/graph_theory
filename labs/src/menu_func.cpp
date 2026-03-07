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
static Matrix *current_matrix = nullptr;

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

void menu_func::StartWorkMenu::calc_eccentricities() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }

    // TODO: Сделать расчет эксцентрисететов
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

void menu_func::StartWorkMenu::make_graph_acyclic() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф!");
        io::wait_enter();
        return;
    }

    size_t n = current_graph->get_size();
    bool is_corrected = false;
    vector<int> degrees(n);

    for (size_t i = 0; i < n; ++i) {
        degrees[i] = (current_graph->degree(i));
    }

#if DEBUG
    string degrees_visualization_before_correcting = "[ ";
    for (int i = 0; i < degrees.size(); i++) {
        degrees_visualization_before_correcting.append(
            std::to_string(degrees[i]));
        if (i != degrees.size() - 1)
            degrees_visualization_before_correcting.append(", ");
        else
            degrees_visualization_before_correcting.append("");
    }
    degrees_visualization_before_correcting.append(" ]");
    io::print_text_with_header(degrees_visualization_before_correcting,
                               "Degrees before correcting", "", BOXED, YELLOW);
#endif

    int sum_deg = 0;
    for (int d : degrees)
        sum_deg += d;
    if (sum_deg % 2 != 0) {
        is_corrected = true;

        degrees[n - 1] += 1;
        sum_deg++;
    }

    if (sum_deg > 2 * (int(n - 1))) {
        is_corrected = true;

        int diff = sum_deg - 2 * (int(n - 1));
        for (size_t i = 0; diff > 0; ++i) {
            size_t idx = i % n;
            if (degrees[idx] > 1) {
                degrees[idx]--;
                diff--;
            }
        }
    }

#if DEBUG
    string degrees_visualization_after_correcting = "[ ";
    for (int i = 0; i < degrees.size(); i++) {
        degrees_visualization_after_correcting.append(
            std::to_string(degrees[i]));
        if (i != degrees.size() - 1)
            degrees_visualization_after_correcting.append(", ");
        else
            degrees_visualization_after_correcting.append("");
    }
    degrees_visualization_after_correcting.append(" ]");
    io::print_text_with_header(degrees_visualization_after_correcting,
                               "Degrees after correcting", "", BOXED, YELLOW);
#endif

    if (is_corrected && current_graph != nullptr) {
        delete current_graph;
        current_graph = new graph(n);
        current_graph->generate(degrees);
    }

    io::print_header("Граф скорректирован: ациклический", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_oriented() {}

void menu_func::StartWorkMenu::run_shimbell() {}

void menu_func::StartWorkMenu::check_routes() {}
