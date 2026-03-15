#pragma once
#include "matrix.h"
#include <vector>

using std::pair;
using std::vector;

enum GraphStatus { NONE = 0, ACYCLIC = 1, ORIENTED = 2 };
enum WeightMode { EMPTY = 0, POSITIVE = 1, NEGATIVE = 2, MIXED = 3 };

class graph {
  private:
    size_t n;

    matrix adj;
    matrix weights;

    int status;
    int mode;

    vector<int>
    get_correct_degrees_for_connected_graph(const vector<int> &degrees);
    vector<int>
    get_correct_degrees_for_acyclic_graph(const vector<int> &degrees);

    void generate_tree(const vector<int> &degrees);
    void generate_connected(const vector<int> &degrees);
    void restore_graph_from_oriented();

    void set_status(GraphStatus s) { status |= s; }
    void clear_status(GraphStatus s) { status &= ~s; }
    void clear_all_statuses() { status = NONE; }

  public:
    graph(size_t vertices);

    void generate_graph();
    void generate_weight_matrix(WeightMode mode);

    void make_graph_acyclic();
    void make_graph_oriented();

    bool has_status(GraphStatus s) const { return status & s; }

    bool is_weight_mode(WeightMode m) const { return mode == m; }
    void set_weight_mode(WeightMode m) { mode |= m; }
    void clear_all_weight_modes() { mode = EMPTY; }

    vector<int> calc_ecc(int start);
    vector<char> calc_central_vertices();
    vector<char> calc_diametral_vertices();

    matrix run_shimbell(size_t edges, bool find_max);

    unsigned long long count_routes(size_t start, size_t end);

    int degree(size_t v) const;
    matrix get_adj() const { return adj; }
    matrix get_weights() const { return weights; }
    size_t get_size() const { return n; }
};
