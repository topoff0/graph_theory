#include "graph.h"
#include "config.h"
#include "distribution.h"
#include "io.h"
#include <algorithm>
#include <climits>
#include <queue>

using std::max;
using std::min;
using std::pair;
using std::queue;

namespace {

vector<int> restore_path(int source, int target, const vector<int> &parent) {
    if (source < 0 || target < 0 || source >= parent.size() ||
        target >= parent.size())
        return {};

    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        path.push_back(v);
        if (v == source)
            break;
    }

    if (path.empty() || path.back() != source)
        return {};

    std::reverse(path.begin(), path.end());
    return path;
}

string path_to_string(const vector<int> &path) {
    if (path.empty())
        return "[ ]";

    string text = "[ ";
    for (size_t i = 0; i < path.size(); i++) {
        text += std::to_string(path[i]);
        if (i + 1 != path.size())
            text += " -> ";
    }
    text += " ]";
    return text;
}

bool is_safe_color(int v, int color, const vector<int> &colors,
                   const matrix &adj) {
    const int n = static_cast<int>(adj.get_rows());
    for (int u = 0; u < n; u++) {
        if (adj.at(v, u) != 0 && colors[u] == color)
            return false;
    }
    return true;
}

bool color_backtracking(int v, int max_colors, vector<int> &colors,
                        const matrix &adj) {
    const int n = static_cast<int>(adj.get_rows());
    if (v == n)
        return true;

    for (int color = 1; color <= max_colors; color++) {
        if (!is_safe_color(v, color, colors, adj))
            continue;
        colors[v] = color;
        if (color_backtracking(v + 1, max_colors, colors, adj))
            return true;
        colors[v] = 0;
    }

    return false;
}

} // namespace

graph::graph(size_t vertices)
    : n(vertices), adj(vertices, vertices), weights(vertices, vertices),
      throughputs(vertices, vertices), costs(vertices, vertices), status(NONE),
      mode(EMPTY), throughputs_matrix_generated(false),
      costs_matrix_generated(false) {}

void graph::generate_tree_from_degrees(const vector<int> &degrees) {
    adj.clear();
    vector<int> deg = degrees;

    while (true) {
        int leaf = -1;
        int other = -1;

        for (int i = 0; i < n; i++)
            if (deg[i] == 1) {
                leaf = i;
                break;
            }

        if (leaf == -1)
            break;

        for (int i = 0; i < n; i++)
            if (deg[i] > 1) {
                other = i;
                break;
            }

        // две последние вершины
        if (other == -1) {
            int u = -1, v = -1;
            for (int i = 0; i < n; i++)
                if (deg[i] == 1) {
                    if (u == -1)
                        u = i;
                    else
                        v = i;
                }
            adj.at(u, v) = 1;
            adj.at(v, u) = 1;
            break;
        }

        adj.at(leaf, other) = 1;
        adj.at(other, leaf) = 1;

        deg[leaf]--;
        deg[other]--;
    }
}

void graph::generate_connected_graph_from_degrees(const vector<int> &degrees) {
    adj.clear();

    vector<pair<int, int>> vertices;

    for (int i = 0; i < n; i++)
        vertices.push_back({degrees[i], i});

    while (true) {
        std::sort(vertices.begin(), vertices.end(),
                  [](auto &a, auto &b) { return a.first > b.first; });

        if (vertices[0].first == 0)
            break;

        int d = vertices[0].first;
        int u = vertices[0].second;

        for (int i = 1; i <= d; i++) {
            int v_idx = vertices[i].second;

            adj.at(u, v_idx) = 1;
            adj.at(v_idx, u) = 1;

            vertices[i].first--;
        }

        vertices[0].first = 0;
    }
}

void graph::generate_DAG_from_degrees(const vector<int> &degrees) {
    adj.clear();
    std::mt19937 rng(static_cast<unsigned>(time(0)));
    vector<int> used(n, 0);

    for (int i = 0; i < n - 1; i++) {
        adj.at(i, i + 1) = 1;
        used[i] = 1;
    }

    for (int i = 0; i < n; i++) {
        int need = degrees[i] - used[i];
        int available = n - i - 1;

        if (need <= 0 || available <= 0)
            continue;

        vector<int> candidates;
        for (int j = i + 1; j < n; j++) {
            if (adj.at(i, j) == 0)
                candidates.push_back(j);
        }

        if (candidates.empty())
            continue;

        std::shuffle(candidates.begin(), candidates.end(), rng);

        int take = std::min(need, (int)candidates.size());

        for (int k = 0; k < take; k++) {
            adj.at(i, candidates[k]) = 1;
        }
    }
}

void graph::restore_graph_from_oriented() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (adj.at(i, j) == 1)
                adj.at(j, i) = 1;
        }
    }
}

void graph::generate_connected_graph() {
    invalidate_flow_matrices();

    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> deg_values = dist.sample(n);

    vector<int> degrees(n, 0);
    for (size_t i = 0; i < n; i++) {
        degrees[i] = static_cast<int>(std::round(deg_values[i]));
    }

    vector<int> deg = get_correct_degrees_for_connected_graph(degrees);

    int sum_deg = 0;
    for (int d : deg)
        sum_deg += d;
    if (sum_deg == 2 * (n - 1)) {
        generate_tree_from_degrees(deg);
        set_status(ACYCLIC);
    } else {
        generate_connected_graph_from_degrees(deg);
        clear_all_statuses();
    }
}

void graph::generate_DAG() {
    invalidate_flow_matrices();
    clear_all_statuses();

    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> deg_values = dist.sample(n);

    vector<int> degrees(n, 0);
    for (size_t i = 0; i < n; i++) {
        degrees[i] = static_cast<int>(std::round(deg_values[i]));
    }
#if DEBUG
    string before = "[ ";
    for (int i = 0; i < degrees.size(); i++) {
        before.append(std::to_string(degrees[i]));
        if (i != degrees.size() - 1)
            before.append(", ");
    }
    before.append(" ]");

    io::print_text_with_header(before, "DEBUG: Степени до корректировки (DAG)",
                               "", BOXED, YELLOW);
#endif

    vector<int> correct_degrees = degrees;
    size_t sz = correct_degrees.size();

    for (size_t i = 0; i < sz; i++) {
        correct_degrees[i] = std::max(1, correct_degrees[i]);
        correct_degrees[i] = std::min(correct_degrees[i], (int)(sz - i - 1));
    }
#if DEBUG
    string after = "[ ";
    for (int i = 0; i < correct_degrees.size(); i++) {
        after.append(std::to_string(correct_degrees[i]));
        if (i != correct_degrees.size() - 1)
            after.append(", ");
    }
    after.append(" ]");

    io::print_text_with_header(
        after, "DEBUG: Степени после корректировки (DAG)", "", BOXED, YELLOW);
#endif

    generate_DAG_from_degrees(correct_degrees);
    set_status(ORIENTED);
    set_status(ACYCLIC);
}

void graph::generate_weight_matrix(const WeightMode mode) {
    set_weight_mode(mode);

    distribution dist(WEIGHT_MU, WEIGHT_ALPHA,
                      static_cast<unsigned int>(time(0)));

    weights.clear();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {

            if (i == j) {
                weights.at(i, j) = 0;
                continue;
            }

            if (adj.at(i, j) == 0) {
                weights.at(i, j) = INT_MAX;
                continue;
            }

            int w = static_cast<int>(std::round(dist.sample()));

            switch (mode) {

            case POSITIVE:
                w = std::abs(w);
                if (w == 0)
                    w = 1;
                break;

            case NEGATIVE:
                w = -std::abs(w);
                if (w == 0)
                    w = -1;
                break;

            case MIXED:
                if (rand() % 2)
                    w = -std::abs(w);
                else
                    w = std::abs(w);
                break;
            case EMPTY:
                return;
            }

            weights.at(i, j) = w;
            if (!has_status(ORIENTED))
                weights.at(j, i) = w;
        }
    }
}

void graph::generate_throughputs_matrix() {
    distribution throughputs_dist(THROUGHPUT_MU, THROUGHPUT_ALPHA,
                                  static_cast<unsigned int>(time(0)));
    throughputs.clear();
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j || adj.at(i, j) == 0) {
                throughputs.at(i, j) = 0;
                continue;
            }

            int throughput = std::abs(
                static_cast<int>(std::round(throughputs_dist.sample())));
            if (throughput == 0)
                throughput = 1;

            throughputs.at(i, j) = throughput;
        }
    }
    throughputs_matrix_generated = true;
}

void graph::generate_costs_matrix() {
    distribution cost_dist(COST_MU, COST_ALPHA,
                           static_cast<unsigned int>(time(0) + 1));
    costs.clear();
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j || adj.at(i, j) == 0) {
                costs.at(i, j) = INT_MAX;
                continue;
            }

            int cost =
                std::abs(static_cast<int>(std::round(cost_dist.sample())));
            if (cost == 0)
                cost = 1;

            costs.at(i, j) = cost;
        }
    }
    costs_matrix_generated = true;
}

void graph::make_graph_acyclic_not_oriented() {
    invalidate_flow_matrices();

    if (has_status(ORIENTED)) {
        restore_graph_from_oriented();
        clear_status(ORIENTED);
    }

    vector<int> degrees(n);

    for (size_t i = 0; i < n; ++i) {
        degrees[i] = (degree(i));
    }

    vector<int> deg = get_correct_degrees_for_acyclic_graph(degrees);
    generate_tree_from_degrees(deg);
    set_status(ACYCLIC);
}

void graph::make_graph_oriented() {
    invalidate_flow_matrices();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < i; j++)
            adj.at(i, j) = 0;

    set_status(ORIENTED);
}

void graph::make_graph_not_oriented() {
    invalidate_flow_matrices();

    if (!has_status(ORIENTED))
        return;

    restore_graph_from_oriented();
    clear_status(ORIENTED);
}

vector<int>
graph::get_correct_degrees_for_connected_graph(const vector<int> &degrees) {

#if DEBUG
    string before = "[ ";
    for (int i = 0; i < degrees.size(); i++) {
        before.append(std::to_string(degrees[i]));
        if (i != degrees.size() - 1)
            before.append(", ");
        else
            before.append("");
    }
    before.append(" ]");
    io::print_text_with_header(before, "DEBUG: Степени до корректировки", "",
                               BOXED, YELLOW);
#endif

    vector<int> correct_degrees = degrees;
    int n = degrees.size();

    for (int &d : correct_degrees)
        d = std::clamp(d, 1, n - 1);

    int sum_deg = 0;
    for (int d : correct_degrees)
        sum_deg += d;

    if (sum_deg % 2 != 0) {
        int max_idx = 0;

        for (int i = 1; i < correct_degrees.size(); i++) {
            if (correct_degrees[i] > correct_degrees[max_idx])
                max_idx = i;
        }

        correct_degrees[max_idx]--;
        sum_deg--;
    }

    int min_sum = 2 * (n - 1);
    while (sum_deg < min_sum) {
        int min_idx = 0;
        int min_idx2 = 0;

        for (int i = 1; i < correct_degrees.size(); i++) {
            if (correct_degrees[i] < correct_degrees[min_idx]) {
                min_idx2 = min_idx;
                min_idx = i;
            }
        }

        correct_degrees[min_idx]++;
        correct_degrees[min_idx2]++;
        sum_deg += 2;
    }

#if DEBUG
    string after = "[ ";
    for (int i = 0; i < correct_degrees.size(); i++) {
        after.append(std::to_string(correct_degrees[i]));
        if (i != correct_degrees.size() - 1)
            after.append(", ");
        else
            after.append("");
    }
    after.append(" ]");
    io::print_text_with_header(after, "DEBUG: Степени после корректировки", "",
                               BOXED, YELLOW);
#endif

    return correct_degrees;
}

vector<int>
graph::get_correct_degrees_for_acyclic_graph(const vector<int> &degrees) {

#if DEBUG
    string before = "[ ";
    for (int i = 0; i < degrees.size(); i++) {
        before.append(std::to_string(degrees[i]));
        if (i != degrees.size() - 1)
            before.append(", ");
    }
    before.append(" ]");

    io::print_text_with_header(
        before, "DEBUG: Степени до корректировки (неациклический граф)", "",
        BOXED, YELLOW);
#endif

    vector<int> correct_degrees = degrees;
    int n = degrees.size();

    int sum_deg = 0;
    for (int d : correct_degrees)
        sum_deg += d;

    int target_sum = 2 * (n - 1);

    while (sum_deg > target_sum) {
        int max_idx = 0;

        for (int i = 1; i < correct_degrees.size(); i++) {
            if (correct_degrees[i] > correct_degrees[max_idx])
                max_idx = i;
        }

        if (correct_degrees[max_idx] > 1) {
            correct_degrees[max_idx]--;
            sum_deg--;
        } else {
            break;
        }
    }

    while (sum_deg < target_sum) {
        int min_idx = 0;

        for (int i = 1; i < correct_degrees.size(); i++) {
            if (correct_degrees[i] < correct_degrees[min_idx])
                min_idx = i;
        }

        correct_degrees[min_idx]++;
        sum_deg++;
    }

#if DEBUG
    string after = "[ ";
    for (int i = 0; i < correct_degrees.size(); i++) {
        after.append(std::to_string(correct_degrees[i]));
        if (i != correct_degrees.size() - 1)
            after.append(", ");
    }
    after.append(" ]");

    io::print_text_with_header(
        after, "DEBUG: Степени после корректировки (ациклический граф)", "",
        BOXED, YELLOW);
#endif

    return correct_degrees;
}

int graph::degree(size_t v) const {
    int d = 0;
    for (size_t i = 0; i < n; i++)
        d += adj.at(v, i);
    return d;
}

vector<int> graph::calc_distances(int start) {
    vector<int> dist(n, -1);
    queue<int> q;

    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int u = 0; u < n; u++) {
            if (adj.at(v, u) == 1 && dist[u] == -1) {
                dist[u] = dist[v] + 1;
                q.push(u);
            }
        }
    }

    return dist;
}

vector<int> graph::calc_ecc() {
    vector<int> eccentricities(n);

    for (int i = 0; i < n; i++) {
        vector<int> dist = calc_distances(i);

        int ecc = 0;
        for (int d : dist) {
            if (d == -1) {
                continue;
            }
            ecc = max(ecc, d);
        }

        eccentricities[i] = ecc;
    }

    return eccentricities;
}

vector<char> graph::calc_central_vertices() {
    vector<int> eccentricities = calc_ecc();
    vector<char> result(n);

    int radius = INT_MAX;
    for (int ecc : eccentricities) {
        radius = min(radius, ecc);
    }

    for (int i = 0; i < n; i++) {
        result[i] = (eccentricities[i] == radius) ? 'x' : '.';
    }

    return result;
}

vector<char> graph::calc_diametral_vertices() {
    vector<int> eccentricities = calc_ecc();
    vector<char> result(n);

    int diameter = 0;
    for (int ecc : eccentricities) {
        diameter = max(diameter, ecc);
    }

    for (int i = 0; i < n; i++) {
        result[i] = (eccentricities[i] == diameter) ? 'x' : '.';
    }

    return result;
}

matrix graph::run_shimbell(size_t edges, bool find_max) {
    if (edges == 0) {
        matrix zero_step(n, n);

        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                if (i == j)
                    zero_step.at(i, j) = 0;
                else
                    zero_step.at(i, j) = INT_MAX;
            }
        }
        return zero_step;
    }

    matrix exact_step_weights = weights;

    for (size_t i = 0; i < n; i++)
        if (adj.at(i, i) == 0)
            exact_step_weights.at(i, i) = INT_MAX;

    if (edges == 1)
        return exact_step_weights;

    matrix result = exact_step_weights;
    matrix temp(n, n);

    for (size_t step = 2; step <= edges; step++) {

        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {

                if (find_max)
                    temp.at(i, j) = INT_MIN;
                else
                    temp.at(i, j) = INT_MAX;

                for (size_t k = 0; k < n; k++) {

                    if (result.at(i, k) == INT_MAX ||
                        exact_step_weights.at(k, j) == INT_MAX)
                        continue;

                    int value = result.at(i, k) + exact_step_weights.at(k, j);

                    if (find_max) {
                        if (value > temp.at(i, j))
                            temp.at(i, j) = value;
                    } else {
                        if (value < temp.at(i, j))
                            temp.at(i, j) = value;
                    }
                }
            }
        }
        result = temp;
    }
    return result;
}

unsigned long long graph::count_routes(size_t start, size_t end) {
    if (start >= n || end >= n)
        return 0;

    vector<unsigned long long> paths(n, 0);
    vector<size_t> in(n, 0);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (adj.at(i, j) == 1)
                in[j]++;
        }
    }

    queue<size_t> zero_in;
    for (size_t i = 0; i < n; i++) {
        if (in[i] == 0)
            zero_in.push(i);
    }

    vector<size_t> t_order;
    while (!zero_in.empty()) {
        size_t v = zero_in.front();
        zero_in.pop();
        t_order.push_back(v);

        for (size_t u = 0; u < n; u++) {
            if (adj.at(v, u) == 1) {
                in[u]--;
                if (in[u] == 0)
                    zero_in.push(u);
            }
        }
    }

    if (t_order.size() != n)
        return 0;

    paths[start] = 1;

    for (size_t v : t_order) {
        if (paths[v] == 0)
            continue;

        for (size_t u = 0; u < n; u++) {
            if (adj.at(v, u) == 1)
                paths[u] += paths[v];
        }
    }

    return paths[end];
}

vector<pair<int, int>> graph::bfs_edges(int start,
                                        unsigned long long &iterations) {
    vector<pair<int, int>> result;
    vector<bool> visited(n, false);
    queue<int> q;
    iterations = 0;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < n; v++) {
            iterations++;

            if (adj.at(u, v) == 0)
                continue;

            result.emplace_back(u, v);

            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    return result;
}

algorithm_result graph::bellman_ford(int start, const matrix *edge_params,
                                     const matrix *available_edges,
                                     bool skip_negative_cycle_check) {
    struct edge {
        size_t from;
        size_t to;
        int weight;
    };

    algorithm_result result;
    result.distances.assign(n, INT_MAX);
    result.parent.assign(n, -1);
    result.has_negative_cycle = false;
    result.iterations = 0;

    vector<edge> edges;
    edges.reserve(n * n);
    vector<vector<size_t>> outgoing_edges(n);

    for (size_t u = 0; u < n; u++) {
        for (size_t v = 0; v < n; v++) {
            if (available_edges->at(u, v) == 0 ||
                edge_params->at(u, v) == INT_MAX)
                continue;

            edges.push_back({u, v, static_cast<int>(edge_params->at(u, v))});
            outgoing_edges[u].push_back(edges.size() - 1);
        }
    }

    result.distances[start] = 0;

    queue<size_t> q;
    vector<bool> in_queue(n, false);
    vector<size_t> relax_count(n, 0);

    q.push(start);
    in_queue[start] = true;

    while (!q.empty()) {
        size_t from = q.front();
        q.pop();
        in_queue[from] = false;

        if (result.distances[from] == INT_MAX)
            continue;

        for (size_t edge_idx : outgoing_edges[from]) {
            const edge &e = edges[edge_idx];
            result.iterations++;

            int candidate = result.distances[from] + e.weight;

            if (candidate < result.distances[e.to]) {
                result.distances[e.to] = candidate;
                result.parent[e.to] = static_cast<int>(from);

                relax_count[e.to]++;

                if (!skip_negative_cycle_check && relax_count[e.to] >= n) {
                    result.has_negative_cycle = true;
                    return result;
                }

                if (!in_queue[e.to]) {
                    q.push(e.to);
                    in_queue[e.to] = true;
                }
            }
        }
    }

    return result;
}

int graph::max_flow_ford_fulkerson(int source, int sink) {
    if (source < 0 || sink < 0 || source >= n || sink >= n || source == sink)
        return 0;

    vector<vector<int>> capacity_left(n, vector<int>(n, 0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            capacity_left[i][j] = static_cast<int>(throughputs.at(i, j));
        }
    }

    int max_flow = 0;

    while (true) {
        vector<int> parent(n, -1);
        queue<int> q;
        q.push(source);
        parent[source] = source;

        while (!q.empty() && parent[sink] == -1) {
            int u = q.front();
            q.pop();

            for (size_t v = 0; v < n; v++) {
                if (parent[v] != -1 || capacity_left[u][v] <= 0)
                    continue;
                parent[v] = u;
                q.push(v);
            }
        }

        if (parent[sink] == -1)
            break;

        int flow_to_add = INT_MAX;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            flow_to_add = min(flow_to_add, capacity_left[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            capacity_left[u][v] -= flow_to_add;
            capacity_left[v][u] += flow_to_add;
        }

        max_flow += flow_to_add;
    }

    return max_flow;
}

algorithm_result graph::min_cost_flow(int source, int sink, int target_flow) {
    algorithm_result result;

    if (source < 0 || sink < 0 || source >= n || sink >= n || source == sink)
        return result;

    vector<vector<int>> capacity_left(n, vector<int>(n, 0));
    matrix r_costs(n, n);
    matrix r_edges(n, n);

    r_costs.fill(INT_MAX);
    r_edges.clear();

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            capacity_left[i][j] = static_cast<int>(throughputs.at(i, j));
            if (adj.at(i, j) == 1) {
                r_costs.at(i, j) = costs.at(i, j);
                r_costs.at(j, i) = -costs.at(i, j);
            }
        }
    }

    vector<string> step_logs;
    vector<int> last_path;
    int steps_count = 0;

    if (target_flow == 0) {
        result.log = "Требуемый поток равен 0";
        return result;
    }

    while (result.flow < target_flow) {
        r_edges.clear();
        for (size_t u = 0; u < n; u++) {
            for (size_t v = 0; v < n; v++) {
                if (capacity_left[u][v] > 0 && r_costs.at(u, v) != INT_MAX) {
                    r_edges.at(u, v) = 1;
                }
            }
        }

        algorithm_result bellman_result =
            bellman_ford(source, &r_costs, &r_edges);

        if (bellman_result.has_negative_cycle) {
            result.has_negative_cycle = true;
            return result;
        }

        if (bellman_result.distances[sink] == INT_MAX)
            break;

        vector<int> path = restore_path(source, sink, bellman_result.parent);
        if (path.empty())
            break;

        int add = target_flow - result.flow;
        for (size_t i = 1; i < path.size(); i++) {
            int u = path[i - 1];
            int v = path[i];
            add = min(add, capacity_left[u][v]);
        }

        int path_cost = 0;
        for (size_t i = 1; i < path.size(); i++) {
            int u = path[i - 1];
            int v = path[i];
            path_cost += static_cast<int>(r_costs.at(u, v));
        }

        for (size_t i = 1; i < path.size(); i++) {
            int u = path[i - 1];
            int v = path[i];
            capacity_left[u][v] -= add;
            capacity_left[v][u] += add;
        }

        result.flow += add;
        result.total_cost += add * path_cost;
        last_path = path;
        steps_count++;

        step_logs.push_back("Шаг " + std::to_string(steps_count) + ": путь " +
                            path_to_string(path) + ", стоимость " +
                            std::to_string(path_cost) + ", поток +" +
                            std::to_string(add));
    }

    result.path = last_path;

    if (step_logs.empty()) {
        result.log = "Подходящий путь для отправки потока не найден";
    } else {
        for (const string &step_log : step_logs)
            result.log += step_log + "\n";

        if (steps_count > step_logs.size()) {
            result.log += "\n...";
            result.log += "\nПропущено шагов: " +
                          std::to_string(steps_count - step_logs.size());
        }
    }

    return result;
}

unsigned long long graph::count_spanning_trees_kirchhoff() const {
    const int size = static_cast<int>(n);
    if (size <= 1)
        return 1;

    vector<vector<long double>> laplacian(
        size, vector<long double>(size, static_cast<long double>(0)));

    for (int i = 0; i < size; i++) {
        int degree_value = 0;
        for (int j = 0; j < size; j++) {
            if (adj.at(i, j) != 0) {
                degree_value++;
                if (i != j)
                    laplacian[i][j] = -1;
            }
        }
        laplacian[i][i] = degree_value;
    }

    const int minor_size = size - 1;
    vector<vector<long double>> minor(minor_size,
                                      vector<long double>(minor_size, 0));

    for (int i = 0; i < minor_size; i++)
        for (int j = 0; j < minor_size; j++)
            minor[i][j] = laplacian[i][j];

    long double det = 1.0L;
    int sign = 1;
    const long double eps = 1e-12L;

    for (int col = 0; col < minor_size; col++) {
        int pivot = col;
        for (int row = col + 1; row < minor_size; row++) {
            if (fabsl(minor[row][col]) > fabsl(minor[pivot][col]))
                pivot = row;
        }

        if (fabsl(minor[pivot][col]) < eps)
            return 0;

        if (pivot != col) {
            std::swap(minor[pivot], minor[col]);
            sign *= -1;
        }

        long double pivot_value = minor[col][col];
        det *= pivot_value;

        for (int row = col + 1; row < minor_size; row++) {
            long double factor = minor[row][col] / pivot_value;
            for (int k = col; k < minor_size; k++)
                minor[row][k] -= factor * minor[col][k];
        }
    }

    det *= sign;
    if (det < 0)
        det = -det;

    return static_cast<unsigned long long>(llround(det));
}

bool graph::build_mst_kruskal(matrix &mst_adj, matrix &mst_weights,
                              vector<weighted_edge> &mst_edges,
                              int &total_weight) const {
    const int size = static_cast<int>(n);
    vector<weighted_edge> edges;

    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (adj.at(i, j) == 0 || weights.at(i, j) == INT_MAX)
                continue;
            edges.push_back({i, j, static_cast<int>(weights.at(i, j))});
        }
    }

    std::sort(edges.begin(), edges.end(),
              [](const weighted_edge &a, const weighted_edge &b) {
                  if (a.weight != b.weight)
                      return a.weight < b.weight;
                  if (a.from != b.from)
                      return a.from < b.from;
                  return a.to < b.to;
              });

    vector<int> parent(size), rank(size, 0);
    std::iota(parent.begin(), parent.end(), 0);

    std::function<int(int)> dsu_find = [&](int v) {
        if (parent[v] == v)
            return v;
        parent[v] = dsu_find(parent[v]);
        return parent[v];
    };

    auto dsu_union = [&](int a, int b) {
        a = dsu_find(a);
        b = dsu_find(b);
        if (a == b)
            return false;
        if (rank[a] < rank[b])
            std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b])
            rank[a]++;
        return true;
    };

    mst_adj.clear();
    mst_weights.fill(INT_MAX);
    for (int i = 0; i < size; i++)
        mst_weights.at(i, i) = 0;

    total_weight = 0;
    mst_edges.clear();

    for (const weighted_edge &edge : edges) {
        if (!dsu_union(edge.from, edge.to))
            continue;

        mst_adj.at(edge.from, edge.to) = 1;
        mst_adj.at(edge.to, edge.from) = 1;
        mst_weights.at(edge.from, edge.to) = edge.weight;
        mst_weights.at(edge.to, edge.from) = edge.weight;

        total_weight += edge.weight;
        mst_edges.push_back(edge);

        if (static_cast<int>(mst_edges.size()) == size - 1)
            break;
    }

    return static_cast<int>(mst_edges.size()) == size - 1;
}

vector<prufer_item>
graph::encode_prufer_with_weights(const matrix &tree_adj,
                                  const matrix &tree_weights,
                                  int &last_edge_weight) const {
    const int size = static_cast<int>(tree_adj.get_rows());
    vector<int> degree(size, 0);
    vector<vector<int>> neighbors(size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (tree_adj.at(i, j) != 0) {
                degree[i]++;
                neighbors[i].push_back(j);
            }
        }
    }

    std::priority_queue<int, vector<int>, std::greater<int>> leaves;
    for (int i = 0; i < size; i++)
        if (degree[i] == 1)
            leaves.push(i);

    vector<prufer_item> code;
    for (int step = 0; step < size - 2; step++) {
        int leaf = leaves.top();
        leaves.pop();

        int neighbor = -1;
        for (int v : neighbors[leaf]) {
            if (degree[v] > 0) {
                neighbor = v;
                break;
            }
        }

        int weight = static_cast<int>(tree_weights.at(leaf, neighbor));
        code.push_back({neighbor, weight});

        degree[leaf]--;
        degree[neighbor]--;

        if (degree[neighbor] == 1)
            leaves.push(neighbor);
    }

    vector<int> left;
    for (int i = 0; i < size; i++)
        if (degree[i] == 1)
            left.push_back(i);

    last_edge_weight = static_cast<int>(tree_weights.at(left[0], left[1]));
    return code;
}

void graph::decode_prufer_with_weights(const vector<prufer_item> &code,
                                       int last_edge_weight,
                                       matrix &decoded_adj,
                                       matrix &decoded_weights) const {
    const int size = static_cast<int>(decoded_adj.get_rows());

    decoded_adj.clear();
    decoded_weights.fill(INT_MAX);
    for (int i = 0; i < size; i++)
        decoded_weights.at(i, i) = 0;

    vector<int> degree(size, 1);
    for (const prufer_item &item : code)
        degree[item.vertex]++;

    std::priority_queue<int, vector<int>, std::greater<int>> leaves;
    for (int i = 0; i < size; i++)
        if (degree[i] == 1)
            leaves.push(i);

    for (const prufer_item &item : code) {
        int leaf = leaves.top();
        leaves.pop();

        int vertex = item.vertex;
        int weight = item.weight;

        decoded_adj.at(leaf, vertex) = 1;
        decoded_adj.at(vertex, leaf) = 1;
        decoded_weights.at(leaf, vertex) = weight;
        decoded_weights.at(vertex, leaf) = weight;

        degree[leaf]--;
        degree[vertex]--;

        if (degree[vertex] == 1)
            leaves.push(vertex);
    }

    vector<int> left;
    for (int i = 0; i < size; i++)
        if (degree[i] == 1)
            left.push_back(i);

    decoded_adj.at(left[0], left[1]) = 1;
    decoded_adj.at(left[1], left[0]) = 1;
    decoded_weights.at(left[0], left[1]) = last_edge_weight;
    decoded_weights.at(left[1], left[0]) = last_edge_weight;
}

bool graph::compare_trees(const matrix &adj_a, const matrix &w_a,
                          const matrix &adj_b, const matrix &w_b) const {
    const int size = static_cast<int>(adj_a.get_rows());
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (adj_a.at(i, j) != adj_b.at(i, j))
                return false;
            if (adj_a.at(i, j) != 0 && w_a.at(i, j) != w_b.at(i, j))
                return false;
        }
    }
    return true;
}

vector<int> graph::chromatic_coloring(const matrix &adj_matrix,
                                      int &chromatic_number) const {
    const int size = static_cast<int>(adj_matrix.get_rows());
    vector<int> colors(size, 0);

    for (int color_count = 1; color_count <= size; color_count++) {
        std::fill(colors.begin(), colors.end(), 0);
        if (color_backtracking(0, color_count, colors, adj_matrix)) {
            chromatic_number = color_count;
            return colors;
        }
    }

    chromatic_number = size;
    return colors;
}
