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

graph::graph(size_t vertices)
    : n(vertices), adj(vertices, vertices), weights(vertices, vertices) {}

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

void graph::make_graph_acyclic_not_oriented() {
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
    for (int i = 0; i < n; i++)
        for (int j = 0; j < i; j++)
            adj.at(i, j) = 0;

    set_status(ORIENTED);
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

vector<pair<int, int>> graph::bfs_edges(int start) {
    vector<pair<int, int>> result;
    vector<bool> visited(n, false);
    queue<int> q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < n; v++) {
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
