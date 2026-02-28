#pragma once

#include "config.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

class helpers
{
public:
    static int count_visible_chars(const string &s);
    static vector<string> split_text(const string &text, int max_line_length = MAX_LINE_LENGTH);
};
