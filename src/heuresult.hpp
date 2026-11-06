#pragma once
#include <filesystem>
#include "instance.hpp"

namespace fs = std::filesystem;

// Result of a heuristic. `name` is the key under which it is written in the
// instance JSON; start_sol is the first (shelves) solution value, obj the final
// height, bound a lower bound and runtime the time in seconds.
struct HeurResult {
    std::string name;
    double start_sol{0.0};
    double obj{0.0};
    double bound{0.0};
    double runtime{0.0};
    Solution sol;

    explicit HeurResult(std::string const& alg_name) : name{alg_name} {}
    // Add the result to the instance JSON file under key `name`.
    void write(fs::path const& file_path);
    void print();
};
