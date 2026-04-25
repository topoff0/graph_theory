struct menu_func {
    struct MainMenu {
        static void test_distribution();
    };

    struct StartWorkMenu {
        static void generate_graph();
        static void generate_acyclic_oriented_graph();
        static void generate_weights_matrix();
        static void generate_throughputs_matrix();
        static void generate_costs_matrix();
        static void make_graph_tree();
        static void make_graph_oriented();
        static void make_graph_not_oriented();
        static void calc_eccentricities();
        static void calc_centers();
        static void calc_diameter();
        static void run_shimbell_method();
        static void check_routes();
        static void edges_bfs();
        static void shortest_path_bellman_ford();
        static void find_max_flow();
        static void find_min_cost_flow();
        static void count_spanning_trees_kirchhoff();
        static void build_mst_kruskal_and_prufer();
        static void min_coloring_for_graph_or_mst();
    };

    static void back_to_main_menu(bool &back);
};
