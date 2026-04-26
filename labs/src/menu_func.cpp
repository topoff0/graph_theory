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
#include <sstream>

static graph *current_graph = nullptr;
static matrix *current_matrix = nullptr;
static matrix *current_matrix_weights = nullptr;

namespace {

void clear_cached_min_ost() {
    if (current_matrix != nullptr) {
        delete current_matrix;
        current_matrix = nullptr;
    }
    if (current_matrix_weights != nullptr) {
        delete current_matrix_weights;
        current_matrix_weights = nullptr;
    }
}

} // namespace

void menu_func::back_to_main_menu(bool &back) { back = true; }

void menu_func::MainMenu::test_distribution() {
    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> values = dist.sample(1000);
    dist.print_histogram(values, 20, COLOR::GREEN, 60);
}

void menu_func::StartWorkMenu::generate_graph() {
    clear_cached_min_ost();

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
    clear_cached_min_ost();

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

    clear_cached_min_ost();

    io::wait_enter();
}

void menu_func::StartWorkMenu::generate_throughputs_matrix() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте ориентированный граф");
        io::wait_enter();
        return;
    }
    if (!current_graph->has_status(ORIENTED)) {
        io::print_error("Граф должен быть ориентированным");
        io::wait_enter();
        return;
    }

    current_graph->generate_throughputs_matrix();
    io::print_header("Сгенерированы матрица пропускных способностей", BOLD);
    io::print_matrix(current_graph->get_throughtputs(),
                     "Матрица пропускных способностей", CYAN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::generate_costs_matrix() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте ориентированный граф");
        io::wait_enter();
        return;
    }
    if (!current_graph->has_status(ORIENTED)) {
        io::print_error("Граф должен быть ориентированным");
        io::wait_enter();
        return;
    }

    current_graph->generate_costs_matrix();
    io::print_header("Сгенерированы матрица стоимости", BOLD);
    io::print_matrix(current_graph->get_costs(), "Матрица стоимости", CYAN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_tree() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    if (current_graph->has_status(TREE)) {
        io::print_error("Текущий граф уже является деревом");
        io::wait_enter();
        return;
    }

    current_graph->make_graph_acyclic_not_oriented();

    io::print_header("Граф скорректирован: дерево", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);
    clear_cached_min_ost();

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
    clear_cached_min_ost();

    io::print_header("Граф скорректирован: ориентированный", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::make_graph_not_oriented() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    if (!current_graph->has_status(ORIENTED)) {
        io::print_error("Граф уже неориентированный");
        io::wait_enter();
        return;
    }

    current_graph->make_graph_not_oriented();
    clear_cached_min_ost();

    io::print_header("Граф скорректирован: неориентированный", BOLD);
    io::print_matrix(current_graph->get_adj(), "Матрица смежности", CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::calc_eccentricities() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

#if DEBUG
    for (int i = 0; i < current_graph->get_size(); i++) {
        vector<int> dist = current_graph->calc_distances(i);
        string text = "[ ";
        for (int i = 0; i < dist.size(); i++) {
            text.append(std::to_string(dist[i]));
            if (i != dist.size() - 1)
                text.append(", ");
        }
        text.append(" ]");
        string header = "DEBUG: Расстояние до вершин от вершины {" +
                        std::to_string(i) + "}";
        io::print_text_with_header(text, header, "", BOXED, YELLOW);
    }

    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif
    vector<int> ecc = current_graph->calc_ecc();
    string text = "[ ";
    for (int i = 0; i < ecc.size(); i++) {
        string text_ecc = ecc[i] == INT_MAX ? "∞" : std::to_string(ecc[i]);
        text.append(text_ecc);
        if (i != ecc.size() - 1)
            text.append(", ");
    }
    text.append(" ]");
    io::print_text_with_header(text, "Эксцентриситеты графа", "", BOXED, GREEN);
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

    int edges = io::read_number({0, current_graph->get_size() - 1},
                                "Введите число ребер");
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
#if DEBUG
    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif

    io::print_text_with_header(text, "Результат проверки маршрутов", "", BOXED,
                               GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::edges_bfs() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    int start = io::read_number({0, current_graph->get_size() - 1},
                                "Введите индекс начальной вершины");

    unsigned long long iterations = 0;
    vector<pair<int, int>> edges =
        current_graph->bfs_edges(static_cast<size_t>(start), iterations);

    string text = "[ ";

    for (size_t i = 0; i < edges.size(); i++) {
        text += "(" + std::to_string(edges[i].first) + ", " +
                std::to_string(edges[i].second) + ")";

        if (i != edges.size() - 1)
            text += ", ";
    }

    text += " ]";

#if DEBUG
    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif

    io::print_text_with_header(text, "Обход рёбер графа (BFS)", "", BOXED,
                               GREEN);

    io::print_text_with_header("Количество итераций: " +
                                   std::to_string(iterations),
                               "BFS", "", BOXED, CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::shortest_path_bellman_ford() {
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

    int start = io::read_number({0, current_graph->get_size() - 1},
                                "Введите индекс начальной вершины");
    int end = io::read_number({0, current_graph->get_size() - 1},
                              "Введите индекс конечной вершины");

    algorithm_result result = current_graph->bellman_ford(
        start, &current_graph->get_weights(), &current_graph->get_adj());

    if (result.has_negative_cycle) {
        io::print_error(
            "В графе найден отрицательный цикл: кратчайшие пути не определены");
        io::wait_enter();
        return;
    }

    string dist_text = "[ ";
    for (size_t i = 0; i < result.distances.size(); i++) {
        if (result.distances[i] == INT_MAX)
            dist_text += "∞";
        else
            dist_text += std::to_string(result.distances[i]);

        if (i != result.distances.size() - 1)
            dist_text += ", ";
    }
    dist_text += " ]";

    io::print_text_with_header(dist_text, "Вектор расстояний (Беллман-Форд)",
                               "", BOXED, GREEN);

    if (result.distances[end] == INT_MAX) {
        io::print_text_with_header("Путь между вершинами отсутствует",
                                   "Кратчайший путь", "", BOXED, YELLOW);
    } else {
        vector<int> path;
        for (int v = end; v != -1; v = result.parent[v])
            path.push_back(v);
        std::reverse(path.begin(), path.end());

        string path_text = "[ ";
        for (size_t i = 0; i < path.size(); i++) {
            path_text += std::to_string(path[i]);
            if (i + 1 != path.size())
                path_text += " -> ";
        }
        path_text += " ]";

        io::print_text_with_header(
            path_text, "Путь: длина = " + std::to_string(result.distances[end]),
            "", BOXED, GREEN);
    }

    io::print_text_with_header("Количество итераций: " +
                                   std::to_string(result.iterations),
                               "Беллман-Форд", "", BOXED, CYAN);

#if DEBUG
    io::print_matrix(current_graph->get_weights(),
                     "DEBUG: Весовая матрица для проверки", YELLOW);

#endif

    io::wait_enter();
}

void menu_func::StartWorkMenu::find_max_flow() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    if (!current_graph->has_status(ORIENTED) ||
        !current_graph->has_status(ACYCLIC)) {
        io::print_error("Для расчета потока граф должен быть ориентированным и "
                        "ациклическим");
        io::wait_enter();
        return;
    }
    if (!current_graph->has_flow_matrices()) {
        io::print_error("Сначала сгенерируйте матрицы пропускных способностей "
                        "и стоимости");
        io::wait_enter();
        return;
    }

    int source = io::read_number({0, current_graph->get_size() - 1},
                                 "Введите индекс вершины-истока");
    int sink = io::read_number({0, current_graph->get_size() - 1},
                               "Введите индекс вершины-стока");

    int max_flow = current_graph->max_flow_ford_fulkerson(source, sink);

#if DEBUG
    io::print_matrix(current_graph->get_throughtputs(),
                     "DEBUG: Матрица пропускных способностей", YELLOW);
#endif
    io::print_text_with_header("Максимальный поток: " +
                                   std::to_string(max_flow),
                               "Форд-Фалкерсон", "", BOXED, GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::find_min_cost_flow() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    if (!current_graph->has_status(ORIENTED) ||
        !current_graph->has_status(ACYCLIC)) {
        io::print_error("Для расчета потока граф должен быть ориентированным и "
                        "ациклическим");
        io::wait_enter();
        return;
    }
    if (!current_graph->has_flow_matrices()) {
        io::print_error("Сначала сгенерируйте матрицы пропускных способностей "
                        "и стоимости");
        io::wait_enter();
        return;
    }

    int source = io::read_number({0, current_graph->get_size() - 1},
                                 "Введите индекс вершины-истока");
    int sink = io::read_number({0, current_graph->get_size() - 1},
                               "Введите индекс вершины-стока");

    int max_flow = current_graph->max_flow_ford_fulkerson(source, sink);
    int target_flow = (2 * max_flow) / 3;
    if (target_flow == 0) {
        io::print_error("Требуемый поток получился равным 0. Увеличьте "
                        "максимальный поток.");
        io::wait_enter();
        return;
    }

    algorithm_result result =
        current_graph->min_cost_flow(source, sink, target_flow);

    if (result.has_negative_cycle) {
        io::print_error("В остаточной сети найден отрицательный цикл");
        io::wait_enter();
        return;
    }

    string text =
        "Требуемый поток: " + std::to_string(target_flow) +
        "\nФактически отправлен: " + std::to_string(result.flow) +
        "\nМинимальная стоимость: " + std::to_string(result.total_cost);

    if (result.flow < target_flow)
        text += "\nНе удалось полностью провести требуемый поток";

#if DEBUG
    io::print_matrix(current_graph->get_throughtputs(),
                     "DEBUG: Матрица пропускных способностей", YELLOW);
#endif
#if DEBUG
    io::print_matrix(current_graph->get_costs(), "DEBUG: Матрица стоимости",
                     YELLOW);
#endif

    io::print_text_with_header(text, "Поток минимальной стоимости", "", BOXED,
                               GREEN);

    io::print_text_with_header(result.log, "Расчет минимальной стоимости", "",
                               BOXED, CYAN);

    io::wait_enter();
}

void menu_func::StartWorkMenu::count_spanning_trees_kirchhoff() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    if (current_graph->has_status(ORIENTED)) {
        io::print_error("Для теоремы Кирхгофа требуется неориентированный граф");
        io::wait_enter();
        return;
    }

    unsigned long long count = current_graph->count_spanning_trees_kirchhoff();
    io::print_text_with_header("Число остовных деревьев: " + std::to_string(count),
                               "Матричная теорема Кирхгофа", "", BOXED, GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::build_mst_kruskal_and_prufer() {
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
    if (current_graph->has_status(ORIENTED)) {
        io::print_error("Для алгоритма Краскала требуется неориентированный граф");
        io::wait_enter();
        return;
    }

    const int n = static_cast<int>(current_graph->get_size());
    matrix min_ost_adj(n, n);
    matrix min_ost_weights(n, n);
    vector<weighted_edge> min_ost_edges;
    int total_weight = 0;

    bool ok = current_graph->build_mst_kruskal(min_ost_adj, min_ost_weights, min_ost_edges,
                                               total_weight);
    if (!ok) {
        io::print_error("Минимальный остов построить нельзя: граф несвязный");
        io::wait_enter();
        return;
    }

    clear_cached_min_ost();
    current_matrix = new matrix(min_ost_adj);
    current_matrix_weights = new matrix(min_ost_weights);

    int last_edge_weight = 0;
    vector<prufer_item> code = current_graph->encode_prufer_with_weights(
        min_ost_adj, min_ost_weights, last_edge_weight);

    matrix decoded_adj(n, n);
    matrix decoded_weights(n, n);
    current_graph->decode_prufer_with_weights(code, last_edge_weight,
                                              decoded_adj, decoded_weights);

    bool decode_ok = current_graph->compare_trees(
        min_ost_adj, min_ost_weights, decoded_adj, decoded_weights);

    std::ostringstream edges_text;
    edges_text << "[ ";
    for (size_t i = 0; i < min_ost_edges.size(); i++) {
        edges_text << "(" << min_ost_edges[i].from << ", " << min_ost_edges[i].to
                   << ", w=" << min_ost_edges[i].weight << ")";
        if (i + 1 != min_ost_edges.size())
            edges_text << ", ";
    }
    edges_text << " ]";

    std::ostringstream code_text;
    code_text << "[ ";
    for (size_t i = 0; i < code.size(); i++) {
        code_text << "(" << code[i].vertex << ", w=" << code[i].weight << ")";
        if (i + 1 != code.size())
            code_text << ", ";
    }
    code_text << " ]";

    io::print_text_with_header(
        "Рёбра остова: " + edges_text.str() +
            "\nСуммарный вес остова: " + std::to_string(total_weight),
        "Краскал: минимальный остов", "", BOXED, GREEN);
    io::print_matrix(min_ost_adj, "Минимальный остов: матрица смежности", CYAN);
    io::print_matrix(min_ost_weights, "Минимальный остов: весовая матрица", CYAN);

    io::print_text_with_header(
        "Код Прюфера (вершина, вес удаляемого ребра): " + code_text.str() +
            "\nВес последнего ребра: " + std::to_string(last_edge_weight),
        "Кодирование Прюфера", "", BOXED, GREEN);
    io::print_matrix(decoded_adj, "Декодированный остов: матрица смежности",
                     CYAN);
    io::print_matrix(decoded_weights, "Декодированный остов: весовая матрица",
                     CYAN);
    io::print_text_with_header(
        decode_ok ? "Декодирование корректно: остов и веса совпадают."
                  : "Ошибка декодирования: результат не совпадает с исходным остовом.",
        "Проверка декодирования", "", BOXED, decode_ok ? GREEN : RED);

    io::wait_enter();
}

void menu_func::StartWorkMenu::min_coloring_for_graph_or_mst() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }

    static const vector<menu_item> COLOR_TARGET_MENU = {
        {1, "Исходный граф", BASE},
        {2, "Полученный остов (MST)", BASE},
    };

    io::print_command_menu(COLOR_TARGET_MENU, "Выберите граф для раскраски");
    int mode = io::read_number(menu_min_max_id(COLOR_TARGET_MENU),
                               "Введите номер варианта");

    const matrix *target_adj = &current_graph->get_adj();
    string target_name = "исходный граф";

    if (mode == 2) {
        if (!current_matrix) {
            io::print_error("Сначала постройте минимальный остов (пункт Краскала)");
            io::wait_enter();
            return;
        }
        target_adj = current_matrix;
        target_name = "полученный остов";
    }

    int chromatic_number = 0;
    vector<int> colors =
        current_graph->chromatic_coloring(*target_adj, chromatic_number);

    std::ostringstream text;
    text << "Хроматическое число: " << chromatic_number << "\nЦвета вершин: [ ";
    for (size_t i = 0; i < colors.size(); i++) {
        text << colors[i];
        if (i + 1 != colors.size())
            text << ", ";
    }
    text << " ]";

    io::print_text_with_header(text.str(),
                               "Минимальная раскраска: " + target_name, "",
                               BOXED, GREEN);
    io::wait_enter();
}
