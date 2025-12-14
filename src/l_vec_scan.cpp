#include "l_vec_scan.hpp"

std::vector<LogLPoint> compute_log_l_vec(ulong q, double t_start, double t_end, double t_step,
                                         slong prec) {
    // 1. Initialize FLINT Group
    dirichlet_group_t G;
    dirichlet_group_init(G, q);

    ulong num_chars = dirichlet_group_size(G);

    // 2. Estimate size and reserve memory
    // Estimate steps to avoid reallocations
    size_t steps = static_cast<size_t>((t_end - t_start) / t_step) + 1;
    std::vector<LogLPoint> results;
    results.reserve(steps * num_chars);

    // 3. Initialize FLINT Variables
    acb_t s;
    acb_init(s);

    // Allocate vector for L-values (size = number of characters)
    acb_ptr l_values = _acb_vec_init(num_chars);

    // Helpers for math operations
    arb_t mag, log_mag;
    arb_init(mag);
    arb_init(log_mag);

    // Structure for optimizing Hurwitz zeta evaluations
    acb_dirichlet_hurwitz_precomp_t precomp;

    // 4. Main Loop over t
    // We add a small epsilon (1e-9) to t_end to handle floating point rounding at the boundary
    for (double t = t_start; t <= t_end + 1e-9; t += t_step) {
        // Set s = 0.5 + i*t
        acb_set_d_d(s, 0.5, t);

        // A. Precomputation Init (Required for the vector function)
        // 0, 0 args let FLINT choose optimal Taylor series depth automatically
        // CORRECT (7 arguments)
        acb_dirichlet_hurwitz_precomp_init(precomp, s, 0, 0, 0, 0, prec);

        // B. Vectorized Calculation (FFT based)
        // Computes L(s, chi) for ALL characters at this specific height t simultaneously
        acb_dirichlet_l_vec_hurwitz(l_values, s, precomp, G, prec);

        // C. Process Results
        for (ulong k = 0; k < num_chars; k++) {
            // Get Magnitude |L(s, chi)|
            acb_abs(mag, l_values + k, prec);

            double val;

            // Handle Zero: log(0) is -inf
            if (arb_is_zero(mag)) {
                val = -std::numeric_limits<double>::infinity();
            } else {
                // Compute log(|L|)
                arb_log(log_mag, mag, prec);
                val = arf_get_d(arb_midref(log_mag), ARF_RND_NEAR);
            }

            // Store result
            results.push_back({t, k, val});
        }

        // D. Cleanup Precomputation for this specific 's'
        acb_dirichlet_hurwitz_precomp_clear(precomp);
    }

    // 5. Final Cleanup
    _acb_vec_clear(l_values, num_chars);
    acb_clear(s);
    arb_clear(mag);
    arb_clear(log_mag);
    dirichlet_group_clear(G);

    return results;
}