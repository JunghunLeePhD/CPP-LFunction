#include "dirichlet_clt_t.hpp"

#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>

#include <cmath>
#include <iostream>
#include <random>

std::vector<std::vector<double>> compute_dirichlet_clt_t_fft(ulong q, double t_start, double t_end,
                                                             int num_samples, slong prec) {
    // 1. Initialize Group
    dirichlet_group_t G;
    dirichlet_group_init(G, q);
    ulong num_chars = dirichlet_group_size(G);

    // 2. Prepare Output Structure
    // storage[char_idx] = list of normalized values
    std::vector<std::vector<double>> storage(num_chars);
    for (ulong k = 0; k < num_chars; ++k) {
        storage[k].reserve(num_samples);
    }

    // 3. FLINT Variables
    acb_t s;
    acb_init(s);

    // Allocate vector for results of ALL characters
    acb_ptr l_values = _acb_vec_init(num_chars);

    arb_t mag, log_mag;
    arb_init(mag);
    arb_init(log_mag);

    acb_dirichlet_hurwitz_precomp_t precomp;

    // 4. Random Generator
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(t_start, t_end);

    // 5. Sampling Loop
    for (int i = 0; i < num_samples; ++i) {
        double current_t = dis(gen);

        // Set s = 0.5 + i*t
        acb_set_d_d(s, 0.5, current_t);

        // A. Precompute Hurwitz parameters (REQUIRED for vec function)
        acb_dirichlet_hurwitz_precomp_init(precomp, s, 0, 0, 0, 0, prec);

        // B. THE FFT STEP
        // Computes L(s, chi) for ALL chi simultaneously
        acb_dirichlet_l_vec_hurwitz(l_values, s, precomp, G, prec);

        // C. Calculate Normalization Factor for this t
        // sigma = sqrt(0.5 * log(log(t)))
        double log_log_t = std::log(std::log(current_t));
        double sigma = std::sqrt(0.5 * log_log_t);
        if (sigma < 1e-9) sigma = 1.0;  // Safety

        // D. Distribute results to each character
        for (ulong k = 0; k < num_chars; k++) {
            // Get |L|
            acb_abs(mag, l_values + k, prec);

            double norm_val = -999.0;

            if (!arb_is_zero(mag)) {
                // log(|L|)
                arb_log(log_mag, mag, prec);
                double raw_val = arf_get_d(arb_midref(log_mag), ARF_RND_NEAR);
                norm_val = raw_val / sigma;
            }

            storage[k].push_back(norm_val);
        }

        // Cleanup precomputation for this t
        acb_dirichlet_hurwitz_precomp_clear(precomp);
    }

    // 6. Cleanup
    _acb_vec_clear(l_values, num_chars);
    acb_clear(s);
    arb_clear(mag);
    arb_clear(log_mag);
    dirichlet_group_clear(G);

    return storage;
}