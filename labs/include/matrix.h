#pragma once
#include <vector>

using std::vector;

class matrix {
  private:
    vector<vector<double>> data;
    size_t rows, cols;

  public:
    matrix(size_t r, size_t c);

    double &at(size_t i, size_t j);
    double at(size_t i, size_t j) const;

    void clear();

    void fill(double value);

    size_t get_rows() const;
    size_t get_cols() const;
};
