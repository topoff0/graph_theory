#pragma once
#include "matrix.h"
#include <vector>

using std::pair;
using std::vector;

class graph {
  private:
    size_t n;
    Matrix adj;

  public:
    graph(size_t vertices);

    void generate(const vector<int> &degrees);
    int degree(size_t v) const;

    Matrix get_adj() const;
    size_t get_size() const;
};
