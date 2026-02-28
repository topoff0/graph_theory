#pragma once

#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

enum COLOR { BASE, GREEN, YELLOW, BLUE, RED, PURPLE, CYAN };

enum HEADER_STYLE { SIMPLE, BOXED, BOLD };

class io {
private:
  // === Border ===
  static string get_line(const int width, const string &s);

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
  static void print_command_menu(const vector<pair<string, COLOR>> &items,
                                 const vector<pair<string, COLOR>> &exit_items,
                                 const string &header);
  static void print_retry(const string &s, COLOR clr = BASE);
  static void print_text_with_header(const string &text,
                                     const string &header = "",
                                     const string &link = "",
                                     HEADER_STYLE style = BOXED,
                                     COLOR color = CYAN);
  // === Error ===
  static void print_error(const string &message, COLOR clr = RED);

  // === Input ===
  static int read_number(int min_value, int max_value, const string &header);
  static void wait_enter();
};
