#pragma once

#include <filesystem>
#include <cmath>

#include "cli.hpp"
#include "instance.hpp"
#include "gurobi_c++.h"


namespace fs = std::filesystem;

struct MIPResult {
    std::string name;
    std::string status;
    double obj{0.0};
    double bound{0.0};
    double gap{0.0};
    double runtime{0.0};
    double buildtime{0.0};

    Solution sol;

    explicit MIPResult(std::string const& alg_name) : name{alg_name} {}

    void write(fs::path const& file_path);
    void print();
};

inline int val(GRBVar const& x) {
    return std::lrint(x.get(GRB_DoubleAttr_X));
}

MIPResult solve(std::string const& name, GRBModel& model, Args const& args);
