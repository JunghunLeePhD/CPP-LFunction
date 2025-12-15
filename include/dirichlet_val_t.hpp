#ifndef DIRICHLET_VAL_T
#define DIRICHLET_VAL_T

#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>
#include <flint/flint.h>

#include <cmath>
#include <limits>
#include <vector>

struct LogLPoint {
    double t;
    ulong char_idx;
    double log_val;
};

std::vector<LogLPoint> compute_val_t_fft(ulong q, double t_start, double t_end, double t_step,
                                         slong prec = 256);

#endif