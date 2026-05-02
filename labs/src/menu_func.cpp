#include "menu_func.h"
#include "colors.h"
#include "config.h"
#include "distribution.h"
#include "graph.h"
#include "io.h"
#include "matrix.h"
#include "menu_item.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>

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

string edge_list_to_string(const vector<weighted_edge> &edges,
                           bool show_weight = true) {
    if (edges.empty())
        return "{ }";

    std::ostringstream text;
    text << "{ ";
    for (size_t i = 0; i < edges.size(); i++) {
        text << "(" << edges[i].from << ", " << edges[i].to;
        if (show_weight)
            text << ", w=" << edges[i].weight;
        text << ")";

        if (i + 1 != edges.size())
            text << ", ";
    }
    text << " }";
    return text.str();
}

string pair_list_to_string(const vector<pair<int, int>> &edges) {
    if (edges.empty())
        return "{ }";

    std::ostringstream text;
    text << "{ ";
    for (size_t i = 0; i < edges.size(); i++) {
        text << "(" << edges[i].first << ", " << edges[i].second << ")";
        if (i + 1 != edges.size())
            text << ", ";
    }
    text << " }";
    return text.str();
}

string vertex_route_to_string(const vector<int> &route) {
    if (route.empty())
        return "[ ]";

    std::ostringstream text;
    text << "[ ";
    for (size_t i = 0; i < route.size(); i++) {
        text << route[i];
        if (i + 1 != route.size())
            text << " -> ";
    }
    text << " ]";
    return text.str();
}

vector<int> parse_cycle_indices(string input, int max_index) {
    for (char &ch : input) {
        if (ch == ',' || ch == ';')
            ch = ' ';
    }

    std::istringstream stream(input);
    vector<int> indices;
    vector<bool> used(max_index, false);
    int value = 0;

    while (stream >> value) {
        if (value < 1 || value > max_index)
            continue;
        if (used[value - 1])
            continue;
        used[value - 1] = true;
        indices.push_back(value - 1);
    }

    return indices;
}

bool current_graph_is_undirected(const string &algorithm_name) {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return false;
    }
    if (current_graph->has_status(ORIENTED)) {
        io::print_error("Для алгоритма \"" + algorithm_name +
                        "\" требуется неориентированный граф");
        io::wait_enter();
        return false;
    }
    return true;
}

bool build_min_tree(matrix &tree_adj, matrix &tree_weights,
                    vector<weighted_edge> &tree_edges, int &total_weight) {
    bool ok = current_graph->build_mst_kruskal(tree_adj, tree_weights,
                                               tree_edges, total_weight);
    if (!ok) {
        io::print_error("Минимальный остов построить нельзя: граф несвязный");
        io::wait_enter();
        return false;
    }

    clear_cached_min_ost();
    current_matrix = new matrix(tree_adj);
    current_matrix_weights = new matrix(tree_weights);
    return true;
}

void print_euler_theorem() {
    string text =
        "Эйлеровых графов почти нет.\n"
        "G(p) - множество всех графов с p вершинами.\n"
        "E(p) - множество эйлеровых графов с p вершинами.\n"
        "При росте p отношение |E(p)| / |G(p)| стремится к 0.";

    io::print_text_with_header(text, "Теорема о количестве эйлеровых графов",
                               "", BOXED, GREEN);
}

string run_euler_theorem_check() {
    std::ostringstream text;

    for (int vertices = EULER_THEOREM_MIN_VERTICES;
         vertices <= EULER_THEOREM_MAX_VERTICES; vertices++) {
        int euler_count = 0;

        for (int i = 0; i < EULER_THEOREM_TESTS_COUNT; i++) {
            graph g(vertices);
            g.generate_DAG(false);
            g.make_graph_not_oriented();

            if (g.is_eulerian())
                euler_count++;
        }

        text << "Вершин: " << vertices << ". Эйлеровых: " << euler_count
             << " из " << EULER_THEOREM_TESTS_COUNT << ".";

        if (vertices != EULER_THEOREM_MAX_VERTICES)
            text << "\n";
    }

    return text.str();
}

} // namespace

void menu_func::back_to_main_menu(bool &back) { back = true; }

void menu_func::MainMenu::test_distribution() {
    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> values = dist.sample(1000);
    dist.print_histogram(values, 20, COLOR::GREEN, 60);
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
        io::print_error(
            "Для теоремы Кирхгофа требуется неориентированный граф");
        io::wait_enter();
        return;
    }

    unsigned long long count = current_graph->count_spanning_trees_kirchhoff();
    io::print_text_with_header("Число остовных деревьев: " +
                                   std::to_string(count),
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
        io::print_error(
            "Для алгоритма Краскала требуется неориентированный граф");
        io::wait_enter();
        return;
    }

    const int n = static_cast<int>(current_graph->get_size());
    matrix min_ost_adj(n, n);
    matrix min_ost_weights(n, n);
    vector<weighted_edge> min_ost_edges;
    int total_weight = 0;

    bool ok = current_graph->build_mst_kruskal(min_ost_adj, min_ost_weights,
                                               min_ost_edges, total_weight);
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

    bool decode_ok = current_graph->compare_trees(min_ost_adj, min_ost_weights,
                                                  decoded_adj, decoded_weights);

    std::ostringstream edges_text;
    edges_text << "[ ";
    for (size_t i = 0; i < min_ost_edges.size(); i++) {
        edges_text << "(" << min_ost_edges[i].from << ", "
                   << min_ost_edges[i].to << ", w=" << min_ost_edges[i].weight
                   << ")";
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
    io::print_matrix(min_ost_weights, "Минимальный остов: весовая матрица",
                     CYAN);

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
                  : "Ошибка декодирования: результат не совпадает с исходным "
                    "остовом.",
        "Проверка декодирования", "", BOXED, decode_ok ? GREEN : RED);

    io::wait_enter();
}

void menu_func::StartWorkMenu::min_coloring_for_graph_or_mst() {
    if (!current_graph) {
        io::print_error("Сначала сгенерируйте граф");
        io::wait_enter();
        return;
    }
    if (current_graph->has_status(ORIENTED)) {
        io::print_error(
            "Для алгоритма минимальной раскраски требуется неориентированный граф");
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
            io::print_error(
                "Сначала постройте минимальный остов (пункт Краскала)");
            io::wait_enter();
            return;
        }
        target_adj = current_matrix;
        target_name = "полученный остов";
    }

    int chromatic_number = 0;
    vector<int> colors =
        current_graph->chromatic_coloring(*target_adj, chromatic_number);

#if DEBUG
    io::print_matrix(current_graph->get_adj(),
                     "DEBUG: Матрица смежности для проверки", YELLOW);
#endif

    std::ostringstream text;
    text << "Хроматическое число: " << chromatic_number << "\nЦвета вершин: [ ";
    for (size_t i = 0; i < colors.size(); i++) {
        text << colors[i];
        if (i + 1 != colors.size())
            text << ", ";
    }
    text << " ]";

    io::print_text_with_header(
        text.str(), "Минимальная раскраска: " + target_name, "", BOXED, GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::build_eulerian_cycle() {
    if (!current_graph_is_undirected("эйлеров цикл"))
        return;

    euler_result result = current_graph->build_eulerian_cycle();
    if (!result.connected) {
        io::print_error(result.log);
        io::wait_enter();
        return;
    }

    if (!result.duplicated_edges.empty())
        clear_cached_min_ost();

    std::ostringstream text;
    text << (result.was_eulerian ? "Граф был эйлеровым"
                                 : "Граф не был эйлеровым")
         << "\n" << result.log
         << "\nПродублированные ребра: "
         << pair_list_to_string(result.duplicated_edges)
         << "\nЭйлеров цикл: " << vertex_route_to_string(result.cycle);

    io::print_text_with_header(text.str(), "Лабораторная 5: эйлеров цикл", "",
                               BOXED, GREEN);
    io::print_matrix(current_graph->get_adj(),
                     "Матрица смежности после проверки/модификации", CYAN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::fundamental_cycles_and_symmetric_difference() {
    if (!current_graph_is_undirected("фундаментальные циклы"))
        return;
    if (current_graph->is_weight_mode(EMPTY)) {
        io::print_error("Сначала сгенерируйте весовую матрицу");
        io::wait_enter();
        return;
    }

    const int n = static_cast<int>(current_graph->get_size());
    matrix min_ost_adj(n, n);
    matrix min_ost_weights(n, n);
    vector<weighted_edge> min_ost_edges;
    int total_weight = 0;

    if (!build_min_tree(min_ost_adj, min_ost_weights, min_ost_edges,
                        total_weight))
        return;

    vector<fundamental_cycle> cycles =
        current_graph->build_fundamental_cycles(min_ost_adj, min_ost_weights);

    std::ostringstream mst_text;
    mst_text << "Ребра остова: " << edge_list_to_string(min_ost_edges)
             << "\nСуммарный вес остова: " << total_weight;

    io::print_text_with_header(mst_text.str(),
                               "Кратчайший остов для системы циклов", "",
                               BOXED, GREEN);
    io::print_matrix(min_ost_adj, "Минимальный остов: матрица смежности", CYAN);

    if (cycles.empty()) {
        io::print_text_with_header(
            "Хорд нет: исходный граф является деревом, фундаментальная система "
            "циклов пуста",
            "Фундаментальная система циклов", "", BOXED, YELLOW);
        io::wait_enter();
        return;
    }

    std::ostringstream cycles_text;
    for (size_t i = 0; i < cycles.size(); i++) {
        cycles_text << "Z" << (i + 1) << " (хорда "
                    << cycles[i].chord_from << "-" << cycles[i].chord_to
                    << "): " << edge_list_to_string(cycles[i].edges);
        if (i + 1 != cycles.size())
            cycles_text << "\n";
    }

    io::print_text_with_header(cycles_text.str(),
                               "Фундаментальная система циклов", "", BOXED,
                               GREEN);

    string input = io::read_string(
        "Введите номера циклов для симметрической разности через пробел");
    vector<int> selected = parse_cycle_indices(input, cycles.size());

    if (selected.empty()) {
        io::print_error("Не выбрано ни одного корректного номера цикла");
        io::wait_enter();
        return;
    }

    vector<weighted_edge> sym_diff =
        current_graph->symmetric_difference_cycles(cycles, selected);

    std::ostringstream selected_text;
    selected_text << "Выбранные циклы: [ ";
    for (size_t i = 0; i < selected.size(); i++) {
        selected_text << "Z" << (selected[i] + 1);
        if (i + 1 != selected.size())
            selected_text << ", ";
    }
    selected_text << " ]\nСимметрическая разность: "
                  << edge_list_to_string(sym_diff);

    io::print_text_with_header(selected_text.str(),
                               "Цикл через симметрическую разность", "",
                               BOXED, GREEN);
    io::wait_enter();
}

void menu_func::StartWorkMenu::check_eulerian_graphs_theorem() {
    print_euler_theorem();

    string result = run_euler_theorem_check();
    io::print_text_with_header(result, "Проверка на сгенерированных графах", "",
                               BOXED, GREEN);
    io::wait_enter();
}
