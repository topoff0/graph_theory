#include "matrix.h"

Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c) {
    data.resize(rows);
    for (size_t i = 0; i < rows; i++)
        data[i].resize(cols, 0.0);
}

double &Matrix::at(size_t i, size_t j) { return data[i][j]; }
double Matrix::at(size_t i, size_t j) const { return data[i][j]; }

size_t Matrix::get_rows() const { return rows; }
size_t Matrix::get_cols() const { return cols; }
