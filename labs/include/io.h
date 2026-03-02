#pragma once
#include "colors.h"
#include "matrix.h"
#include "menu_item.h"

#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

enum HEADER_STYLE { SIMPLE, BOXED, BOLD };

class io {
  private:
    // === Border ===
    static string get_border(const int width, const string &s);

    // === Colors ===
    const static string reset_color;
    const static string get_color_code(COLOR clr);

    // === Header ===
    static void print_simple_header(const string &s, const string &main_color);
    static void print_boxed_header(const string &s, const string &main_color);
    static void print_bold_header(const string &s, const string &main_color);

  public:
    // === Align ===
    static void v_space(int times);

    // === Output ===
    static void print(const string &s, COLOR clr = BASE);
    static void print_header(const string &s, HEADER_STYLE style = SIMPLE,
                             COLOR clr = COLOR::CYAN);
    static void print_list(const vector<pair<string, COLOR>> &items,
                           const string &header);
    static void print_command_menu(const vector<menu_item> &items,
                                   const string &header);
    static void print_retry(const string &s, COLOR clr = BASE);

    static void print_text_with_header(const string &text,
                                       const string &header = "",
                                       const string &link = "",
                                       HEADER_STYLE style = BOXED,
                                       COLOR color = BASE);

    // === Current lab ===
    static void print_matrix(const Matrix &mat, const std::string &header = "",
                             COLOR clr = BASE);

    // === Error ===
    static void print_error(const string &message, COLOR clr = RED);

    // === Input ===
    static int read_number(const pair<int, int> &min_max_id,
                           const string &header);
    static string read_string(const string &header);
    static void wait_enter();
};
