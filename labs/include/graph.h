#pragma once
#include "matrix.h"
#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

enum GraphStatus { NONE = 0, ACYCLIC = 1, ORIENTED = 2, TREE = 4 };
enum WeightMode { EMPTY = 0, POSITIVE = 1, NEGATIVE = 2, MIXED = 4 };

struct algorithm_result {
    vector<int> distances;
    vector<int> parent;
    vector<int> path;
    int flow = 0;
    int total_cost = 0;
    unsigned long long iterations = 0;
    bool has_negative_cycle = false;
    string log;
};

class graph {
  private:
    size_t n;

    matrix adj;
    matrix weights;
    matrix throughputs;
    matrix costs;

    int status;
    int mode;

    bool throughputs_matrix_generated;
    bool costs_matrix_generated;

    vector<int>
    get_correct_degrees_for_connected_graph(const vector<int> &degrees);
    vector<int>
    get_correct_degrees_for_acyclic_graph(const vector<int> &degrees);

    void generate_connected_graph_from_degrees(const vector<int> &degrees);
    void generate_DAG_from_degrees(const vector<int> &degrees);
    void generate_tree_from_degrees(const vector<int> &degrees);

    void restore_graph_from_oriented();

    void set_status(GraphStatus s) { status |= s; }
    void clear_status(GraphStatus s) { status &= ~s; }
    void clear_all_statuses() { status = NONE; }

    void invalidate_flow_matrices() {
        throughputs_matrix_generated = false;
        costs_matrix_generated = false;
    }

  public:
    graph(size_t vertices);

    void generate_connected_graph();
    void generate_DAG();
    void generate_weight_matrix(WeightMode mode);
    void generate_throughputs_matrix();
    void generate_costs_matrix();

    bool has_flow_matrices() const {
        return throughputs_matrix_generated && costs_matrix_generated;
    }

    void make_graph_acyclic_not_oriented();
    void make_graph_oriented();

    bool has_status(GraphStatus s) const { return status & s; }

    bool is_weight_mode(WeightMode m) const { return mode == m; }
    void set_weight_mode(WeightMode m) { mode |= m; }
    void clear_all_weight_modes() { mode = EMPTY; }

    vector<int> calc_distances(int start);
    vector<int> calc_ecc();
    vector<char> calc_central_vertices();
    vector<char> calc_diametral_vertices();

    matrix run_shimbell(size_t edges, bool find_max);

    vector<pair<int, int>> bfs_edges(int start, unsigned long long &iterations);
    algorithm_result bellman_ford(int start,
                                  const matrix *edge_params,
                                  const matrix *available_edges,
                                  bool skip_negative_cycle_check = false);
    int max_flow_ford_fulkerson(int source, int sink);
    algorithm_result min_cost_flow(int source, int sink, int target_flow);

    unsigned long long count_routes(size_t start, size_t end);

    int degree(size_t v) const;
    const matrix& get_adj() const { return adj; }
    const matrix& get_weights() const { return weights; }
    size_t get_size() const { return n; }
    const matrix& get_throughtputs() const { return throughputs; }
    const matrix& get_costs() const { return costs; }
};
