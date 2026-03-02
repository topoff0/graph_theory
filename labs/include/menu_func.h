struct menu_func {
    struct MainMenu {
        static void test_distribution();
    };

    struct StartWorkMenu {
        static void generate_graph();
        static void calc_eccentricities();
        static void calc_centers();
        static void calc_diameter();
        static void run_shimbell();
        static void check_routes();
    };

    static void back_to_main_menu(bool &back);
};
