#ifndef DIRICHLET_CLT_T
#define DIRICHLET_CLT_T

#include <flint/flint.h>

#include <vector>

struct SelbergSample {
    double t;
    double raw_log_val;
    double normalized_val;
};

std::vector<std::vector<double>> compute_dirichlet_clt_t_fft(ulong q, double t_start, double t_end,
                                                             int num_samples, slong prec);

#endif