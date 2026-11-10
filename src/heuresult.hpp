#pragma once
#include <filesystem>
#include "instance.hpp"

namespace fs = std::filesystem;

// Result of a heuristic: `name` is the instance name, start_sol the first
// (shelves) solution value, obj the final height, bound a lower bound and
// runtime the time in seconds. print() outputs obj, bound and runtime as JSON.
struct HeurResult {
    std::string name;
    double start_sol{0.0};
    double obj{0.0};
    double bound{0.0};
    double runtime{0.0};
    Solution sol;

    explicit HeurResult(std::string const& inst_name) : name{inst_name} {}
    // Print the result to stdout as a JSON object.
    void print();
};
