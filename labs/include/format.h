#pragma once

enum ALIGN { ALIGN_LEFT, ALIGN_CENTER, ALIGN_JUSTIFY };

#include "config.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

class format {
  public:
    static int count_visible_chars(const string &s);
    static vector<string> split_text(const string &text,
                                     int max_line_length = MAX_LINE_LENGTH,
                                     ALIGN align = ALIGN_JUSTIFY);

  private:
    static string align_center(const string &line, int width);
    static string align_justify(const string &line, int width);
};
