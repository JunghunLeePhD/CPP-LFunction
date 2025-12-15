#include <omp.h>

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "crow.h"
#include "dirichlet_clt_t.hpp"
#include "l_functions_flint.hpp"
#include "l_vec_scan.hpp"

int main() {
    crow::SimpleApp app;

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

    CROW_ROUTE(app, "/dirichlet_clt_t")
    ([](const crow::request&, crow::response& res) {
        // Make sure this file exists in your build/static folder
        res.set_static_file_info("static/dirichlet_clt_t/index.html");
        res.end();
    });

    CROW_ROUTE(app, "/dirichlet_clt_t_data")
    ([](const crow::request& req) {
        ulong q = req.url_params.get("q") ? std::stoul(req.url_params.get("q")) : 7;
        int samples =
            req.url_params.get("samples") ? std::stoi(req.url_params.get("samples")) : 5000;
        double T = req.url_params.get("T") ? std::stod(req.url_params.get("T")) : 1000.0;
        double t_start = T;
        double t_end = 2.0 * T;

        // This returns data for ALL characters at once
        std::vector<std::vector<double>> all_data =
            compute_dirichlet_clt_t_fft(q, t_start, t_end, samples, 64);

        // Pack into JSON
        std::vector<crow::json::wvalue> all_datasets;
        all_datasets.reserve(all_data.size());

        for (size_t idx = 0; idx < all_data.size(); idx++) {
            std::vector<double>& raw_vals = all_data[idx];

            // Filter out -999 errors if necessary
            std::vector<double> clean_vals;
            clean_vals.reserve(raw_vals.size());
            for (double v : raw_vals) {
                if (v > -900.0) clean_vals.push_back(v);
            }

            crow::json::wvalue ds;
            ds["label"] = idx;
            ds["values"] = std::move(clean_vals);
            all_datasets.push_back(std::move(ds));
        }

        crow::json::wvalue response;
        response["datasets"] = std::move(all_datasets);
        response["range_start"] = t_start;
        response["range_end"] = t_end;

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