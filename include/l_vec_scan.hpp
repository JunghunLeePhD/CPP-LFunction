#ifndef L_VEC_SCAN_HPP
#define L_VEC_SCAN_HPP

#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>
#include <flint/flint.h>

#include <cmath>
#include <limits>  // For infinity handling
#include <vector>

// Structure to hold one data point: (t, character_index, value)
struct LogLPoint {
    double t;
    ulong char_idx;
    double log_val;
};

// Function signature
std::vector<LogLPoint> compute_log_l_vec(ulong q, double t_start, double t_end, double t_step,
                                         slong prec = 256);

#endif  // L_VEC_SCAN_HPP