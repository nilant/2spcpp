#pragma once

#include <gurobi_c++.h>

#include "instance.hpp"
#include "mdarray.hpp"
#include "mipresult.hpp"
#include "cli.hpp"


// Matching MIP used by the matheuristic: given the cost matrix `combs`
// (combs(i, j) = height obtained by merging sub-instances i and j), pick the
// perfect matching of minimum total cost. x(i, j), i < j, is 1 when i and j
// are paired.
class Assignment {
    std::string name;  // instance name, reported in the result
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 2> x;

    public:
        Assignment(GRBEnv& env, std::string const& inst_name, mdarray<int, 2> const& combs);
        MIPResult optimize(Args const& args);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
        // Return the merged instances (from `instances`) of the selected pairs.
        std::vector<Instance> select(mdarray<Instance, 2> const& instances);
};