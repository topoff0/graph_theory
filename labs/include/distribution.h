#pragma once
#include "colors.h"
#include <random>
#include <vector>

using std::vector;

class distribution {
  private:
    double _mu;
    double _alpha;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;

  public:
    distribution(double mu, double alpha,
                 unsigned int seed = std::random_device{}());

    double sample();
    vector<double> sample(size_t n);

    void print_histogram(const vector<double> &values, size_t steps = 10,
                         COLOR color = GREEN, size_t width = 50);
};
