#include <omp.h>

#include <string>
#include <vector>

#include "crow.h"
#include "l_functions_flint.hpp"

int main() {
    crow::SimpleApp app;

    // API Route for computation
    CROW_ROUTE(app, "/scan_flint")
    ([](const crow::request& req) {
        // 1. Parse Parameters
        double r = req.url_params.get("r") ? std::stod(req.url_params.get("r")) : 0.5;
        double start = req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 0.0;
        double end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 30.0;
        int steps = req.url_params.get("steps") ? std::stoi(req.url_params.get("steps")) : 100;
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 1;
        unsigned long input_idx =
            req.url_params.get("idx") ? std::stoul(req.url_params.get("idx")) : 1;

        ulong idx = (input_idx > 0) ? (input_idx - 1) : 0;

        // 2. Pre-allocate vector
        std::vector<ComplexResult> raw_results(steps + 1);
        double step_size = (end - start) / steps;

// 3. OpenMP Parallel Loop
#pragma omp parallel for schedule(dynamic)
        for (int k = 0; k <= steps; ++k) {
            double current_t = start + (k * step_size);

            // Call the function from l_functions.cpp
            raw_results[k] = compute_l_raw(r, current_t, q, idx, 64);
        }

        // 4. Convert to JSON
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

    // Root Route - Serve the HTML from static folder
    CROW_ROUTE(app, "/")
    ([](const crow::request&, crow::response& res) {
        res.set_static_file_info("static/index.html");
        res.end();
    });

    app.port(8080).multithreaded().run();
}