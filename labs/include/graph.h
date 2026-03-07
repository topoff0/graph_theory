#pragma once
#include "matrix.h"
#include <vector>

using std::pair;
using std::vector;

class graph {
  private:
    size_t n;
    Matrix adj;

    vector<int>
    get_correct_degrees_for_connected_graph(const vector<int> &degrees);
    vector<int>
    get_correct_degrees_for_acyclic_graph(const vector<int> &degrees);

  public:
    graph(size_t vertices);

    void generate(const vector<int> &degrees);
    void make_graph_acyclic(const vector<int> &degrees);
    void make_graph_oriented();
    int degree(size_t v) const;

    Matrix get_adj() const;
    size_t get_size() const;
};
