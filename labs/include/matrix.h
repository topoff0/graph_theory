#pragma once
#include <vector>

using std::vector;

class Matrix {
  private:
    vector<vector<double>> data;
    size_t rows, cols;

  public:
    Matrix(size_t r, size_t c);

    double &at(size_t i, size_t j);
    double at(size_t i, size_t j) const;

    size_t get_rows() const;
    size_t get_cols() const;
};
