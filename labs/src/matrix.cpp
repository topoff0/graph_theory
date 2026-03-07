#include "matrix.h"

matrix::matrix(size_t r, size_t c) : rows(r), cols(c) {
    data.resize(rows);
    for (size_t i = 0; i < rows; i++)
        data[i].resize(cols, 0.0);
}

double &matrix::at(size_t i, size_t j) { return data[i][j]; }
double matrix::at(size_t i, size_t j) const { return data[i][j]; }

void matrix::clear() {
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            data[i][j] = 0.0;
}

size_t matrix::get_rows() const { return rows; }
size_t matrix::get_cols() const { return cols; }
