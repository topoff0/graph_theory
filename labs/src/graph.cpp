#include "graph.h"
#include "config.h"
#include "io.h"
#include <algorithm>

using std::pair;

graph::graph(size_t vertices) : n(vertices), adj(vertices, vertices) {}

void graph::generate_with_correct_degrees(const vector<int> &degrees) {
    vector<pair<int, int>> vertices;

    adj.clear();

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

void graph::generate(const vector<int> &degrees) {
    vector<int> deg = get_correct_degrees_for_connected_graph(degrees);

    generate_with_correct_degrees(deg);
}

void graph::make_graph_acyclic(const vector<int> &degrees) {
    vector<int> deg = get_correct_degrees_for_acyclic_graph(degrees);

    generate_with_correct_degrees(deg);
}

void graph::make_graph_oriented() {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < i; j++)
            adj.at(i, j) = 0;
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
    io::print_text_with_header(before, "Degrees before correcting", "", BOXED,
                               YELLOW);
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
    io::print_text_with_header(after, "Degrees after correcting", "", BOXED,
                               YELLOW);
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

    io::print_text_with_header(before, "Degrees before correcting (acyclic)",
                               "", BOXED, YELLOW);
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

    io::print_text_with_header(after, "Degrees after correcting (acyclic)", "",
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
