#ifndef L_FUNCTIONS_FLINT_HPP
#define L_FUNCTIONS_FLINT_HPP

#include <flint/flint.h>

// 1. Define the struct so both files know what a "Result" looks like
struct ComplexResult {
    double real;
    double imag;
    double t;
};

// 2. Declare the function (Prototype)
// We use 'ulong' and 'slong' which are FLINT types
ComplexResult compute_l_raw(double s_real, double s_imag, ulong q, ulong char_idx, slong prec);

#endif  // L_FUNCTIONS_HPP