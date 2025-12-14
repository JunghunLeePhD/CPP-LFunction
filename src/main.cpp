#include <omp.h>

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "crow.h"
#include "l_functions_flint.hpp"
#include "l_vec_scan.hpp"
#include "selberg_clt.hpp"

int main() {
    crow::SimpleApp app;

    // --- Route 1: Scan ALL Characters (FFT Based) ---
    CROW_ROUTE(app, "/scan_all")
    ([](const crow::request& req) {
        // 1. Parse Parameters
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 7;
        double start = req.url_params.get("start") ? std::stod(req.url_params.get("start")) : 0.0;
        double end = req.url_params.get("end") ? std::stod(req.url_params.get("end")) : 30.0;

        // Determine step size based on range to keep point count reasonable
        // e.g., target ~500 points per character
        double range = end - start;
        double steps = 500.0;
        double step_size = range / steps;
        if (req.url_params.get("step")) {
            step_size = std::stod(req.url_params.get("step"));
        }

        // 2. Compute using the Vectorized FFT function
        // This returns a flat list of {t, char_idx, val}
        std::vector<LogLPoint> raw_data = compute_log_l_vec(q, start, end, step_size, 64);

        // 3. Reorganize data for JSON: Group by Character Index
        // Map: char_idx -> list of values
        std::map<ulong, std::vector<double>> char_map;
        std::vector<double> t_values;

        // We assume t values are consistent across characters (they are in the loop).
        // To avoid duplicate t arrays, we extract t once.
        bool t_collected = false;
        ulong num_chars = 0;

        // First pass: find number of characters
        for (const auto& p : raw_data) {
            if (p.char_idx > num_chars) num_chars = p.char_idx;
        }
        num_chars++;  // 0-indexed

        // Pre-allocate map vectors
        for (ulong c = 0; c < num_chars; ++c) {
            char_map[c].reserve(raw_data.size() / num_chars);
        }

        // Fill data
        double last_t = -99999.0;
        for (const auto& p : raw_data) {
            // Check if we moved to a new t-step
            if (std::abs(p.t - last_t) > 1e-9) {
                t_values.push_back(p.t);
                last_t = p.t;
            }

            // Handle -inf (log of 0) by clamping to a low value for charting
            double safe_val = (p.log_val < -100) ? -10.0 : p.log_val;

            char_map[p.char_idx].push_back(safe_val);
        }

        // 4. Construct JSON Response
        crow::json::wvalue response;
        response["t"] = std::move(t_values);

        std::vector<crow::json::wvalue> datasets;
        for (auto const& [c_idx, values] : char_map) {
            crow::json::wvalue ds;
            ds["label"] = c_idx;  // "Character 0", "Character 1", etc.
            ds["data"] = values;
            datasets.push_back(std::move(ds));
        }

        response["datasets"] = std::move(datasets);
        return response;
    });

    // --- Route 2: Selberg CLT (Existing) ---
    CROW_ROUTE(app, "/selberg_clt")
    ([](const crow::request& req) {
        // ... (Keep your existing Selberg logic here) ...
        // For brevity, I am omitting the body, paste your previous Selberg code here
        return crow::json::wvalue();
    });

    // Root Route
    CROW_ROUTE(app, "/")
    ([](const crow::request&, crow::response& res) {
        res.set_static_file_info("static/index.html");
        res.end();
    });

    app.port(8080).multithreaded().run();
}