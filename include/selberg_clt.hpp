#ifndef SELBERG_CLT_HPP
#define SELBERG_CLT_HPP

#include <flint/flint.h>

#include <vector>

// Structure to hold simulation data
struct SelbergSample {
    double t;
    double raw_log_val;     // log|L(s)|
    double normalized_val;  // log|L(s)| / sqrt(0.5 * log log t)
};

// Function prototype
std::vector<SelbergSample> compute_selberg_samples(ulong q, ulong char_idx, double t_start,
                                                   double t_end, int num_samples, slong prec);

#endif  // SELBERG_CLT_HPP