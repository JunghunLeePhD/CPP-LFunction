#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>
#include <flint/flint.h>
#include <omp.h>  // <--- 1. Include OpenMP

#include "crow.h"

struct ComplexResult {
    double real;
    double imag;
    double t;
};

auto compute_l_raw = [](double s_real, double s_imag, ulong q, ulong char_idx,
                        slong prec) -> ComplexResult {
    acb_t s, res, z_real, z_imag;
    acb_init(s);
    acb_init(res);
    acb_init(z_real);
    acb_init(z_imag);
    arb_t r, i;
    arb_init(r);
    arb_init(i);

    acb_set_d_d(s, s_real, s_imag);

    dirichlet_group_t G;
    dirichlet_group_init(G, q);

    dirichlet_char_t chi;
    dirichlet_char_init(chi, G);

    // 2. UPDATE: Use the passed index instead of hardcoded 0
    // Note: char_idx should be < phi(q). FLINT handles this via group structure.
    dirichlet_char_index(chi, G, char_idx);

    acb_dirichlet_l(res, s, G, chi, prec);

    acb_get_real(r, res);
    acb_get_imag(i, res);

    ComplexResult out;
    out.real = arf_get_d(arb_midref(r), ARF_RND_NEAR);
    out.imag = arf_get_d(arb_midref(i), ARF_RND_NEAR);
    out.t = s_imag;

    dirichlet_char_clear(chi);
    dirichlet_group_clear(G);
    acb_clear(s);
    acb_clear(res);
    acb_clear(z_real);
    acb_clear(z_imag);
    arb_clear(r);
    arb_clear(i);

    return out;
};

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/scan")
    ([](const crow::request& req) {
        double r = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double start = req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 0.0;
        double end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 30.0;
        int steps = req.url_params.get("steps") ? std::stoi(req.url_params.get("steps")) : 100;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;
        unsigned long input_idx =
            req.url_params.get("idx") ? std::stoul(req.url_params.get("idx")) : 1;

        ulong idx = (input_idx > 0) ? (input_idx - 1) : 0;

        // 2. Pre-allocate vector (Important for thread safety!)
        // We use a raw struct vector first to avoid JSON overhead in the hot loop
        std::vector<ComplexResult> raw_results(steps + 1);

        double step_size = (end - start) / steps;

// 3. OpenMP Parallel Loop
// "dynamic" scheduling is better here because some L-values take longer to converge than others
#pragma omp parallel for schedule(dynamic)
        for (int k = 0; k <= steps; ++k) {
            double current_t = start + (k * step_size);

            // This function is thread-safe because it uses local variables
            raw_results[k] = compute_l_raw(r, current_t, q, idx, 64);
        }

        // 4. Convert to JSON (Serial part, very fast)
        std::vector<crow::json::wvalue> json_results;
        json_results.reserve(steps + 1);

        for (const auto& val : raw_results) {
            crow::json::wvalue p;
            p["t"] = val.t;
            p["real"] = val.real;
            p["imag"] = val.imag;
            json_results.push_back(std::move(p));
        }

        crow::json::wvalue final_json;
        final_json["points"] = std::move(json_results);
        return final_json;
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load_text("index.html");
    });

    // Ensure Crow uses multiple threads for web requests too
    app.port(8080).multithreaded().run();
}