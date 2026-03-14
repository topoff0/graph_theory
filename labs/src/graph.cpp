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

void graph::generate_tree(const vector<int> &degrees) {
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

void graph::generate_connected(const vector<int> &degrees) {
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

        vertices.erase(vertices.begin());
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

void graph::generate_graph() {
    distribution dist(MU, ALPHA, static_cast<unsigned int>(time(0)));
    vector<double> deg_values = dist.sample(n);

    vector<int> deg(n, 0);
    for (size_t i = 0; i < n; i++) {
        deg[i] = static_cast<int>(std::round(deg_values[i]));
    }

    int sum_deg = 0;
    for (int d : deg)
        sum_deg += d;
    if (sum_deg == 2 * (n - 1)) {
        generate_tree(deg);
        set_status(ACYCLIC);
    } else {
        generate_connected(deg);
        clear_all_statuses();
    }
}
void graph::generate_weight_matrix(const WeightMode mode) {
    set_weight_mode(mode);

    distribution dist(WEIGHT_MU, WEIGHT_ALPHA, static_cast<unsigned int>(time(0)));
}

void graph::make_graph_acyclic() {
    if (has_status(ORIENTED)) {
        restore_graph_from_oriented();
        clear_status(ORIENTED);
    }

    vector<int> degrees(n);

    for (size_t i = 0; i < n; ++i) {
        degrees[i] = (degree(i));
    }

    vector<int> deg = get_correct_degrees_for_acyclic_graph(degrees);
    generate_tree(deg);
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

    int min_sum = n - 1;
    while (sum_deg < min_sum) {
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

vector<int> graph::calc_ecc(int start) {
    vector<int> eccentricities(n, -1);
    queue<int> q;

    eccentricities[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int u = 0; u < n; u++) {
            if (adj.at(v, u) == 1 && eccentricities[u] == -1) {
                eccentricities[u] = eccentricities[v] + 1;
                q.push(u);
            }
        }
    }

    return eccentricities;
}

vector<char> graph::calc_central_vertices() {
    int radius = INT_MAX;
    vector<int> eccentricities(n);
    vector<char> result(n);

    for (int i = 0; i < n; i++) {
        vector<int> current_eccentircities = calc_ecc(i);
        int eccentricity = 0;

        for (int j = 0; j < n; j++) {
            if (current_eccentircities[j] == -1) {
                eccentricity = INT_MAX;
                break;
            }
            eccentricity = max(eccentricity, current_eccentircities[j]);
        }

        radius = min(radius, eccentricity);
        eccentricities[i] = eccentricity;
    }

    for (int i = 0; i < n; i++) {
        result[i] = (eccentricities[i] == radius) ? 'x' : '.';
    }

    return result;
}

vector<char> graph::calc_diametral_vertices() {
    int diameter = 0;
    vector<int> eccentricities(n);
    vector<char> result(n);

    for (int i = 0; i < n; i++) {
        vector<int> dist = calc_ecc(i);
        int eccentricity = 0;

        for (int j = 0; j < n; j++) {
            if (dist[j] == -1) {
                eccentricity = INT_MAX;
                break;
            }
            eccentricity = max(eccentricity, dist[j]);
        }

        eccentricities[i] = eccentricity;
        diameter = max(diameter, eccentricity);
    }

    for (int i = 0; i < n; i++) {
        result[i] = (eccentricities[i] == diameter) ? 'x' : '.';
    }

    return result;
}
