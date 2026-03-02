#include "graph.h"
#include <queue>
using std::pair;

graph::graph(size_t vertices) : n(vertices), adj(vertices, vertices) {}

void graph::generate(const vector<int> &degrees) {
    std::vector<std::pair<int, size_t>> vertices;
    for (size_t i = 0; i < n; ++i) {
        std::pair<int, size_t> p;
        p.first = degrees[i];
        p.second = i;
        vertices.push_back(p);
    }

    while (true) {
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t max_idx = i;
            for (size_t j = i + 1; j < vertices.size(); ++j) {
                if (vertices[j].first > vertices[max_idx].first) {
                    max_idx = j;
                }
            }
            if (max_idx != i) {
                std::pair<int, size_t> tmp = vertices[i];
                vertices[i] = vertices[max_idx];
                vertices[max_idx] = tmp;
            }
        }

        while (!vertices.empty() && vertices.back().first == 0) {
            vertices.pop_back();
        }

        if (vertices.empty()) {
            break;
        }

        int d = vertices[0].first;
        size_t v_idx = vertices[0].second;

        for (int i = 1; i <= d; ++i) {
            int u_deg = vertices[i].first;
            size_t u_idx = vertices[i].second;

            adj.at(v_idx, u_idx) = 1.0;
            adj.at(u_idx, v_idx) = 1.0;

            vertices[i].first = u_deg - 1;
        }

        vertices.erase(vertices.begin());
    }
}

int graph::degree(size_t v) const {
    int d = 0;
    for (size_t i = 0; i < n; i++)
        d += adj.at(v, i);
    return d;
}


Matrix graph::get_adj() const { return adj; }

size_t graph::get_size() const { return n; }

