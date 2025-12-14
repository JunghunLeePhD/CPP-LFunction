#ifndef SELBERG_CLT_HPP
#define SELBERG_CLT_HPP

#include <flint/flint.h>

#include <vector>

struct SelbergSample {
    double t;
    double raw_log_val;
    double normalized_val;
};

std::vector<std::vector<double>> compute_selberg_samples_fft(ulong q, double t_start, double t_end,
                                                             int num_samples, slong prec);

#endif