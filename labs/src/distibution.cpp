#include "distribution.h"
#include "io.h"
#include <string>

distribution::distribution(double mu, double alpha, unsigned int seed)
    : _mu(mu), _alpha(alpha), rng(seed), dist(0.0, 1.0) {}

double distribution::sample() {
    double r = dist(rng);

    return _mu + (1.0 / _alpha) * std::log(std::tan(M_PI * r / 2.0));
}

vector<double> distribution::sample(size_t n) {
    vector<double> values;
    values.reserve(n);

    for (size_t i = 0; i < n; ++i)
        values.push_back(sample());

    return values;
}

void distribution::print_histogram(const vector<double> &values, size_t steps,
                                   COLOR color, size_t width) {
    if (values.empty() || steps == 0)
        return;

    double min_val = values[0];
    double max_val = values[0];

    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] < min_val)
            min_val = values[i];
        if (values[i] > max_val)
            max_val = values[i];
    }

    if (min_val == max_val)
        return;

    double step_size = (max_val - min_val) / steps;

    vector<size_t> counts(steps);

    for (size_t i = 0; i < steps; ++i)
        counts[i] = 0;

    for (size_t i = 0; i < values.size(); ++i) {
        size_t idx = (values[i] - min_val) / step_size;

        if (idx >= steps)
            idx = steps - 1;

        counts[idx]++;
    }

    size_t max_count = counts[0];
    for (size_t i = 1; i < steps; ++i) {
        if (counts[i] > max_count)
            max_count = counts[i];
    }

    io::v_space(1);
    io::print_header("Гистограмма распределения", HEADER_STYLE::BOXED, CYAN);

    for (size_t i = 0; i < steps; ++i) {
        double left = min_val + i * step_size;
        double right = left + step_size;

        size_t bar_len = static_cast<size_t>(
            (static_cast<double>(counts[i]) / max_count) * width);

        std::string line = std::to_string(left).substr(0, 4) + " ─ " +
                           std::to_string(right).substr(0, 4) + " │ ";

        for (size_t j = 0; j < bar_len; ++j)
            line += "█";

        io::print(line, color);
    }

    io::v_space(1);
}
