#pragma once
#include "matrix.h"
#include <vector>

using std::pair;
using std::vector;

enum GraphStatus { NONE = 0, ACYCLIC = 1, ORIENTED = 2 };

class graph {
  private:
    size_t n;
    matrix adj;
    int status;

    vector<int>
    get_correct_degrees_for_connected_graph(const vector<int> &degrees);
    vector<int>
    get_correct_degrees_for_acyclic_graph(const vector<int> &degrees);

    void generate_tree(const vector<int> &degrees);
    void generate_connected(const vector<int> &degrees);
    void restore_graph_from_oriented();

    void set_status(GraphStatus s) { status |= s; }
    void clear_status(GraphStatus s) { status &= ~s; }
    void clear_all_status() { status = NONE; }

  public:
    graph(size_t vertices);

    void generate(const vector<int> &degrees);
    void make_graph_acyclic();
    void make_graph_oriented();
    int degree(size_t v) const;

    bool has_status(GraphStatus s) const { return status & s; }

    vector<int> bfs_ecc(int start = 0);

    matrix get_adj() const { return adj; }
    size_t get_size() const { return n; }
};
