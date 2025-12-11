#include <omp.h>

#include <cmath>  // for std::isnan check if needed
#include <string>
#include <vector>

#include "crow.h"
#include "l_functions_flint.hpp"
#include "selberg_clt.hpp"

int main() {
    crow::SimpleApp app;

    // --- Existing Route: Scan (Linear) ---
    CROW_ROUTE(app, "/scan_flint")
    ([](const crow::request& req) {
        // ... (Keep your existing code for /scan_flint here) ...
        // (Just ensure you paste your existing logic here)

        // Brief placeholder for your existing logic:
        double r = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double start = req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 0.0;
        double end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 30.0;
        int steps = req.url_params.get("steps") ? std::stoi(req.url_params.get("steps")) : 100;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;
        unsigned long input_idx =
            req.url_params.get("idx") ? std::stoul(req.url_params.get("idx")) : 1;
        ulong idx = (input_idx > 0) ? (input_idx - 1) : 0;

        std::vector<ComplexResult> raw_results(steps + 1);
        double step_size = (end - start) / steps;

#pragma omp parallel for schedule(dynamic)
        for (int k = 0; k <= steps; ++k) {
            double current_t = start + (k * step_size);
            raw_results[k] = compute_l_raw(r, current_t, q, idx, 64);
        }

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

    // --- NEW Route: Selberg CLT Visualization ---
    CROW_ROUTE(app, "/selberg_clt")
    ([](const crow::request& req) {
        // 1. Parse Parameters
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;
        unsigned long input_idx =
            req.url_params.get("idx") ? std::stoul(req.url_params.get("idx")) : 1;
        ulong idx = (input_idx > 0) ? (input_idx - 1) : 0;

        // Range for random sampling (Higher T is better for CLT convergence)
        double t_start =
            req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 1000.0;
        double t_end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 2000.0;
        int samples =
            req.url_params.get("samples") ? std::stoi(req.url_params.get("samples")) : 5000;

        // 2. Compute Samples
        // We do this in a single call since the function handles the loop efficiently.
        // If samples > 100,000, you might want to chunk this with OpenMP inside the CPP file.
        std::vector<SelbergSample> data =
            compute_selberg_samples(q, idx, t_start, t_end, samples, 64);

        // 3. Convert to JSON
        // We return the raw normalized values so the Frontend (JS) can bin them into a histogram.
        std::vector<crow::json::wvalue> json_data;
        json_data.reserve(data.size());

        for (const auto& item : data) {
            crow::json::wvalue p;
            p["t"] = item.t;
            p["val"] = item.normalized_val;  // This is what goes into the histogram
            json_data.push_back(std::move(p));
        }

        crow::json::wvalue response;
        response["samples"] = std::move(json_data);
        response["q"] = q;
        response["idx"] = idx;

        return response;
    });

    // Root Route
    CROW_ROUTE(app, "/")
    ([](const crow::request&, crow::response& res) {
        res.set_static_file_info("static/index.html");
        res.end();
    });

    app.port(8080).multithreaded().run();
}