#include "l_functions.hpp"  // Include our own header

#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>

// Implementation of the function
ComplexResult compute_l_raw(double s_real, double s_imag, ulong q, ulong char_idx, slong prec) {
    acb_t s, res;
    acb_init(s);
    acb_init(res);

    // Setup temporary variables for extraction
    arb_t r, i;
    arb_init(r);
    arb_init(i);

    acb_set_d_d(s, s_real, s_imag);

    dirichlet_group_t G;
    dirichlet_group_init(G, q);

    dirichlet_char_t chi;
    dirichlet_char_init(chi, G);

    // Set the character index
    dirichlet_char_index(chi, G, char_idx);

    // Compute L-function
    acb_dirichlet_l(res, s, G, chi, prec);

    // Extract Real and Imag parts
    acb_get_real(r, res);
    acb_get_imag(i, res);

    ComplexResult out;
    out.real = arf_get_d(arb_midref(r), ARF_RND_NEAR);
    out.imag = arf_get_d(arb_midref(i), ARF_RND_NEAR);
    out.t = s_imag;

    // Cleanup memory
    dirichlet_char_clear(chi);
    dirichlet_group_clear(G);
    acb_clear(s);
    acb_clear(res);
    arb_clear(r);
    arb_clear(i);

    return out;
}