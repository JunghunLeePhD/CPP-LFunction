#include "selberg_clt.hpp"

#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>

#include <cmath>
#include <iostream>
#include <random>

std::vector<SelbergSample> compute_selberg_samples(ulong q, ulong char_idx, double t_start,
                                                   double t_end, int num_samples, slong prec) {
    std::vector<SelbergSample> results;
    results.reserve(num_samples);

    // 1. Initialize FLINT/Arb variables once
    acb_t s, res;
    acb_init(s);
    acb_init(res);

    arb_t arb_t_val, arb_half, arb_log_mod, arb_norm_factor;
    arb_init(arb_t_val);
    arb_init(arb_half);
    arb_init(arb_log_mod);
    arb_init(arb_norm_factor);

    // Set Real part = 0.5
    arb_set_d(arb_half, 0.5);

    // 2. Initialize Dirichlet Group and Character ONCE
    // This is much faster than re-initializing inside the loop
    dirichlet_group_t G;
    dirichlet_group_init(G, q);

    dirichlet_char_t chi;
    dirichlet_char_init(chi, G);
    dirichlet_char_index(chi, G, char_idx);

    // 3. Setup Random Number Generator
    // We use thread_local to ensure thread safety if called from OpenMP in main
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(t_start, t_end);

    // 4. Sampling Loop
    for (int i = 0; i < num_samples; ++i) {
        double current_t = dis(gen);

        // Set s = 0.5 + i*t
        arb_set_d(arb_t_val, current_t);
        acb_set_arb_arb(s, arb_half, arb_t_val);

        // Compute L(s, chi)
        acb_dirichlet_l(res, s, G, chi, prec);

        // Compute log|L(s)|
        // |L(s)|
        acb_abs(arb_log_mod, res, prec);
        // log(|L(s)|)
        arb_log(arb_log_mod, arb_log_mod, prec);

        // Compute Variance: sigma = sqrt(0.5 * log(log(t)))
        double log_log_t = std::log(std::log(current_t));
        double sigma = std::sqrt(0.5 * log_log_t);

        // Extract raw value to double
        double raw_val = arf_get_d(arb_midref(arb_log_mod), ARF_RND_NEAR);

        // Normalize
        double norm_val = raw_val / sigma;

        results.push_back({current_t, raw_val, norm_val});
    }

    // 5. Cleanup
    dirichlet_char_clear(chi);
    dirichlet_group_clear(G);
    acb_clear(s);
    acb_clear(res);
    arb_clear(arb_t_val);
    arb_clear(arb_half);
    arb_clear(arb_log_mod);
    arb_clear(arb_norm_factor);

    return results;
}