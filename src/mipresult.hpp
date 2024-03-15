#pragma once

#include <filesystem>
#include "cli.hpp"
#include "gurobi_c++.h"


namespace fs = std::filesystem;

struct MIPResult {
    std::string name;
    double obj;
    double bound;
    double gap;
    double runtime;
    double buildtime;

    explicit MIPResult(std::string const& alg_name) : name{alg_name} {}

    void write(fs::path const& file_path);
    void print();
};

MIPResult solve(std::string const& name, GRBModel& model, Args const& args, int ub);