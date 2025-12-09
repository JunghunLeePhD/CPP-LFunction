#include <flint/acb.h>
#include <flint/acb_dirichlet.h>
#include <flint/arb.h>
#include <flint/flint.h>

#include "crow.h"

struct ComplexResult {
    double real;
    double imag;
    double t;
};

auto compute_l_raw = [](double s_real, double s_imag, ulong q, slong prec) -> ComplexResult {
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
    dirichlet_char_index(chi, G, 0);

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

    CROW_ROUTE(app, "/calc")
    ([](const crow::request& req) {
        double r = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double i = req.url_params.get("i") ? std::stod(req.url_params.get("i")) : 14.1347;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;

        auto res = compute_l_raw(r, i, q, 128);

        crow::json::wvalue x;
        x["real"] = res.real;
        x["imag"] = res.imag;
        return x;
    });

    CROW_ROUTE(app, "/scan")
    ([](const crow::request& req) {
        double r = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double start = req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 0.0;
        double end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 30.0;
        int steps = req.url_params.get("steps") ? std::stoi(req.url_params.get("steps")) : 100;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;

        std::vector<crow::json::wvalue> results;
        double step_size = (end - start) / steps;

        for (int k = 0; k <= steps; ++k) {
            double current_t = start + (k * step_size);
            auto val = compute_l_raw(r, current_t, q, 64);  // Lower precision for speed

            crow::json::wvalue p;
            p["t"] = val.t;
            p["real"] = val.real;
            p["imag"] = val.imag;
            results.push_back(p);
        }

        crow::json::wvalue final_json;
        final_json["points"] = std::move(results);
        return final_json;
    });

    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        return crow::mustache::load_text("index.html");
    });

    app.port(8080).multithreaded().run();
}