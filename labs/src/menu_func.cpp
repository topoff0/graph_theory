#include "menu_func.h"
#include "config.h"
#include "distribution.h"

void menu_func::back_to_main_menu(bool &back) { back = true; }

void menu_func::MainMenu::test_distribution() {
    distribution dist(MU, ALPHA);
    auto values = dist.sample(1000);

    dist.print_histogram(values, 20, COLOR::GREEN, 60);
}
