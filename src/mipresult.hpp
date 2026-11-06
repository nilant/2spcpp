#pragma once

#include <filesystem>
#include <cmath>

#include "cli.hpp"
#include "instance.hpp"
#include "gurobi_c++.h"


namespace fs = std::filesystem;

// Result of a MIP solve: objective, best bound, relative gap, solve and model
// build time (seconds). All values are -1 when no solution was found.
struct MIPResult {
    std::string name;
    double obj{0.0};
    double bound{0.0};
    double gap{0.0};
    double runtime{0.0};
    double buildtime{0.0};

    Solution sol;

    explicit MIPResult(std::string const& alg_name) : name{alg_name} {}

    // Print the result to stdout as a JSON object.
    void print();
};

// Rounded value of a binary/integer variable in the current solution.
inline int val(GRBVar const& x) {
    return std::lrint(x.get(GRB_DoubleAttr_X));
}

// Apply the command line limits to `model`, optimize it and collect the result.
MIPResult solve(std::string const& name, GRBModel& model, Args const& args);
