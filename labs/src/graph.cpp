#include "graph.h"
#include "config.h"
#include "io.h"
#include <algorithm>

graph::graph(size_t vertices) : n(vertices), adj(vertices, vertices) {}

void graph::generate(const vector<int> &degrees) {
    vector<int> deg = get_correct_degrees_for_connected_graph(degrees);

    vector<int> remaining = deg;

    while (true) {
        int u = -1, v = -1;

        for (int i = 0; i < n; i++)
            if (remaining[i] > 0)
                u = i;

        if (u == -1)
            break;

        for (int j = 0; j < n; j++) {
            if (j != u && remaining[j] > 0 && adj.at(u, j) == 0) {
                v = j;
                break;
            }
        }

        if (v == -1)
            break;

        adj.at(u, v) = 1;
        adj.at(v, u) = 1;

        remaining[u]--;
        remaining[v]--;
    }
}

void graph::make_graph_acyclic(const vector<int> &degrees) {}

vector<int>
graph::get_correct_degrees_for_connected_graph(const vector<int> &degrees) {

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

    vector<int> correct_degrees = degrees;
    int n = degrees.size();

    for (int &d : correct_degrees)
        d = std::clamp(d, 0, n - 1);

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
    string degrees_visualization_after_correcting = "[ ";
    for (int i = 0; i < correct_degrees.size(); i++) {
        degrees_visualization_after_correcting.append(
            std::to_string(correct_degrees[i]));
        if (i != correct_degrees.size() - 1)
            degrees_visualization_after_correcting.append(", ");
        else
            degrees_visualization_after_correcting.append("");
    }
    degrees_visualization_after_correcting.append(" ]");
    io::print_text_with_header(degrees_visualization_after_correcting,
                               "Degrees after correcting", "", BOXED, YELLOW);
#endif

    return correct_degrees;
}

vector<int>
graph::get_correct_degrees_for_acyclic_graph(const vector<int> &degrees) {}

int graph::degree(size_t v) const {
    int d = 0;
    for (size_t i = 0; i < n; i++)
        d += adj.at(v, i);
    return d;
}

Matrix graph::get_adj() const { return adj; }

size_t graph::get_size() const { return n; }
