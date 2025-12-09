#include "crow.h"

// FLINT 3 Includes
#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>
#include <flint/flint.h>

auto arb_to_string = [](const acb_t& z, slong digits) -> std::string {
    // 1. Prepare variables for Real (r) and Imaginary (i) parts
    arb_t r, i;
    arb_init(r);
    arb_init(i);

    // 2. Extract parts from the complex number z
    acb_get_real(r, z);
    acb_get_imag(i, z);

    // 3. Convert each part to a C-string
    // arb_get_str arguments: (arb_t x, slong n, ulong flags)
    char* s_r = arb_get_str(r, digits, 0);
    char* s_i = arb_get_str(i, digits, 0);

    // 4. Format nicely as "a + bi"
    std::string result = std::string(s_r) + " + " + std::string(s_i) + "i";

    // 5. Cleanup memory
    flint_free(s_r);
    flint_free(s_i);
    arb_clear(r);
    arb_clear(i);

    return result;
};

auto compute_l_function = [](double s_real, double s_imag, ulong q, slong prec) -> std::string {
    acb_t s, res;
    acb_init(s);
    acb_init(res);

    // Set s = s_real + i*s_imag
    acb_set_d_d(s, s_real, s_imag);

    dirichlet_group_t G;
    dirichlet_group_init(G, q);

    dirichlet_char_t chi;
    dirichlet_char_init(chi, G);
    dirichlet_char_index(chi, G, 0);

    // Compute L(s, chi)
    acb_dirichlet_l(res, s, G, chi, prec);

    // Convert result to string
    std::string output = arb_to_string(res, 15);

    dirichlet_char_clear(chi);
    dirichlet_group_clear(G);
    acb_clear(s);
    acb_clear(res);

    return output;
};

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/calc")
    ([](const crow::request& req) {
        double s_real = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double s_imag = req.url_params.get("i") ? std::stod(req.url_params.get("i")) : 14.1347;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;

        std::string result = compute_l_function(s_real, s_imag, q, 128);

        crow::json::wvalue x;
        x["result"] = result;
        x["modulus"] = q;
        x["s_real"] = s_real;
        x["s_imag"] = s_imag;

        return x;
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load_text("index.html");
    });

    app.port(8080).multithreaded().run();
}